#include "virtual_fileystem.h"

#include "ifilesystem.h"

namespace loss
{
    VirtualFileSystem::VirtualFileSystem() : 
        _root_filesystem(nullptr),
        _filesystem_counter(0u)
    {

    }

    ReturnCode VirtualFileSystem::root_filesystem(IFileSystem *fs)
    {
        auto result = register_file_system(fs);
        if (result != SUCCESS && result != ALREADY_IN_LIST)
        {
            return result;
        }

        _root_filesystem = fs;
        return SUCCESS;
    }
    IFileSystem *VirtualFileSystem::root_filesystem() const
    {
        return _root_filesystem;
    }

    const VirtualFileSystem::FileSystems &VirtualFileSystem::file_systems() const
    {
        return _file_systems;
    }

    ReturnCode VirtualFileSystem::find_handles_for_processes(uint32_t process_id, VirtualFileSystem::FileHandles &result) const
    {
        const auto find = _process_to_id.find(process_id);
        if (find == _process_to_id.end())
        {
            return CANNOT_FIND_PROCESS;
        }

        for (auto iter_id : find->second)
        {
            const auto find_handle = _id_to_file_handle.find(iter_id);
            if (find_handle == _id_to_file_handle.end())
            {
                return INTERNAL_ERROR;
            }

            const auto &handles = find_handle->second;
            for (auto iter = handles.begin(); iter != handles.end(); ++iter)
            {
                if (iter->get()->process_id() == process_id)
                {
                    result.push_back(iter->get());
                }
            }
        }
        return SUCCESS;
    }
    ReturnCode VirtualFileSystem::close_process_handles(uint32_t process_id)
    {
        FileHandles handles;
        auto result = find_handles_for_processes(process_id, handles);
        if (result != SUCCESS)
        {
            return result;
        }

        for (auto iter : handles)
        {
            close(iter);
        }

        return SUCCESS;
    }
    
    ReturnCode VirtualFileSystem::register_file_system(IFileSystem *fs)
    {
        if (fs == nullptr)
        {
            return NULL_PARAMETER;
        }

        for (auto iter = _file_systems.begin(); iter != _file_systems.end(); ++iter)
        {
            if (iter->get() == fs)
            {
                return ALREADY_IN_LIST;
            }
        }

        fs->filesystem_id(++_filesystem_counter);
        _file_systems.push_back(std::unique_ptr<IFileSystem>(fs));
        return SUCCESS;
    }
    
    ReturnCode VirtualFileSystem::open(uint32_t process_id, const std::string &name, FileHandle::OpenMode open_mode, FileHandle *&handle)
    {
        if (name.empty())
        {
            return NULL_PARAMETER;
        }

        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return ENTRY_NOT_FOUND;
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.status() != SUCCESS)
        {
            return ENTRY_NOT_FOUND;
        }

        /*
         * Handle this differently!
        if ((open_mode | FileHandle::WRITE) && find_write_handle(find.id(), find.fs()))
        {
            return FILE_HAS_WRITE_LOCK;
        }
        */

        handle = new FileHandle(find.id(), process_id, open_mode, result.fs());
        auto id = make_id(find.id(), find.fs());
        _id_to_file_handle[id].push_back(std::unique_ptr<FileHandle>(handle)); 
        _process_to_id[process_id].push_back(id);
        return SUCCESS;
    }
    ReturnCode VirtualFileSystem::close(FileHandle *handle)
    {
        if (handle == nullptr)
        {
            return NULL_PARAMETER;
        }

        auto id = make_id(handle->entry_id(), handle->filesystem());

        // Remove process to id map
        const auto find_process_id = _process_to_id.find(handle->process_id());
        if (find_process_id == _process_to_id.end())
        {
            return CANNOT_FIND_PROCESS;
        }
        auto &ids = find_process_id->second;
        for (auto iter = ids.begin(); iter != ids.end(); ++iter)
        {
            if (*iter == id)
            {
                ids.erase(iter);
                break;
            }
        }

        // Remove file handle
        const auto find_process_handle = _id_to_file_handle.find(id);
        if (find_process_handle == _id_to_file_handle.end())
        {
            return CANNOT_FIND_PROCESS;
        }

        auto &handles = find_process_handle->second;
        for (auto iter = handles.begin(); iter != handles.end(); ++iter)
        {
            if (iter->get() == handle)
            {
                handles.erase(iter);
                return SUCCESS;
            }
        }
        return CANNOT_FIND_FILE_HANDLE;
    }

    IOResult VirtualFileSystem::read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return IOResult(0, result.status());
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.metadata().type() == SYMLINK_ENTRY)
        {
            std::cout << "Attempting to read symlink\n";
        }

        if (find.status() != SUCCESS)
        {
            return IOResult(0, find.status());
        }
        return result.fs()->read(find.id(), offset, count, buffer);
    }
    IOResult VirtualFileSystem::read(FileHandle *handle, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (handle == nullptr || buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (!handle->has_read_mode())
        {
            return IOResult(0, HANDLE_LACKING_READ);
        }
        return handle->filesystem()->read(handle->entry_id(), offset, count, buffer);
    }
    ReturnCode VirtualFileSystem::read_folder(const std::string &name, FolderEntry &folder)
    {
        Path path(name);
        path.filename_to_dir();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        return result.fs()->read_folder(result.id(), folder);
    }

    IOResult VirtualFileSystem::read_stream(FileHandle *handle, uint32_t offset, uint32_t count, std::ostream &ss)
    {
        // TODO Handle special case of -1 count
        uint8_t *temp = new uint8_t[count];
        auto result = read(handle, offset, count, temp); 
        if (result.status() == SUCCESS && result.bytes() > 0)
        {
            ss.write(reinterpret_cast<const char *>(temp), result.bytes());
        }
        delete []temp;

        return result;
    }
    IOResult VirtualFileSystem::read_stream(const std::string &name, uint32_t offset, uint32_t count, std::ostream &ss)
    {
        // TODO Handle special case of -1 count
        uint8_t *temp = new uint8_t[count];
        auto result = read(name, offset, count, temp); 
        if (result.status() == SUCCESS && result.bytes() > 0)
        {
            ss.write(reinterpret_cast<const char *>(temp), result.bytes());
        }
        delete []temp;

        return result;
    }

    IOResult VirtualFileSystem::write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return IOResult(0, result.status());
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.status() != SUCCESS)
        {
            return IOResult(0, find.status());
        }
        return result.fs()->write(find.id(), offset, count, data);
    }
    IOResult VirtualFileSystem::write(FileHandle *handle, uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (handle == nullptr || data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (!handle->has_write_mode())
        {
            return IOResult(0, HANDLE_LACKING_WRITE);
        }
        return handle->filesystem()->write(handle->entry_id(), offset, count, data);
    }
    IOResult VirtualFileSystem::write_string(const std::string &name, uint32_t offset, const std::string &data)
    {
        return write(name, offset, data.size(), reinterpret_cast<const uint8_t *>(data.c_str()));
    }
    IOResult VirtualFileSystem::write_string(FileHandle *handle, uint32_t offset, const std::string &data)
    {
        return write(handle, offset, data.size(), reinterpret_cast<const uint8_t *>(data.c_str()));
    }

    ReturnCode VirtualFileSystem::entry_size(const std::string &name, uint32_t &size)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.status() != SUCCESS)
        {
            return find.status();
        }
        return result.fs()->entry_size(find.id(), size);
    }

    ReturnCode VirtualFileSystem::entry_metadata(const std::string &name, MetadataDef &metadata)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.status() != SUCCESS)
        {
            return find.status();
        }
        return result.fs()->entry_metadata(find.id(), metadata);
    }

    ReturnCode VirtualFileSystem::update_entry_metadata(const std::string &name, const MetadataDef &metadata)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.status() != SUCCESS)
        {
            return find.status();
        }
        return result.fs()->update_entry_metadata(find.id(), metadata);
    }

    ReturnCode VirtualFileSystem::create_file(const std::string &name)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        return result.fs()->create_file(result.id(), path.filename()).status();
    }
    ReturnCode VirtualFileSystem::create_symlink(const std::string &name, const std::string &link)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        return result.fs()->create_symlink(result.id(), path.filename(), link).status();
    
    }
    ReturnCode VirtualFileSystem::create_folder(const std::string &name)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        return result.fs()->create_folder(result.id(), path.filename()).status();
    }
    ReturnCode VirtualFileSystem::create_char_device(const std::string &name, ICharacterDevice *device)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        return result.fs()->create_char_device(result.id(), path.filename(), device).status();
    }
    

    ReturnCode VirtualFileSystem::mount(const std::string &name, IFileSystem *fs)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        return result.fs()->mount(result.id(), path.filename(), fs).status();
    }

    ReturnCode VirtualFileSystem::remove_entry(const std::string &name)
    {
        Path path(name);
        path.dir_to_filename();

        auto result = follow_path(path, IFileSystem::ROOT_ID);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        auto find = result.fs()->find_entry(result.id(), path.filename());
        if (find.status() != SUCCESS)
        {
            return find.status();
        }
        return result.fs()->remove_entry(find.id());
    }

    FindEntryResult VirtualFileSystem::follow_path(const Path &path, uint32_t folder_id)
    {
        if (_root_filesystem == nullptr)
        {
            return FindEntryResult(0, VFS_HAS_NO_ROOT, nullptr);
        }

        auto fs = _root_filesystem;
        for (auto dir : path.dirs())
        {
            auto result = fs->find_entry(folder_id, dir);
            if (result.status() != SUCCESS)
            {
                return result;
            }

            fs = result.fs();
            if (result.metadata().type() == SYMLINK_ENTRY)
            {
                std::string symlink;
                auto symlink_result = fs->read_symlink(result.id(), symlink);
                if (symlink_result != SUCCESS)
                {
                    return FindEntryResult(folder_id, symlink_result, fs);
                }
                std::cout << "Symlink: " << symlink << "\n";
            }

            // Do something with symlinks!
            folder_id = result.id();
        }

        return FindEntryResult(folder_id, SUCCESS, fs);
    }

    bool VirtualFileSystem::find_write_handle(uint32_t entry_id, IFileSystem *fs) const
    {
        uint64_t id = make_id(entry_id, fs);
        const auto find = _id_to_file_handle.find(id);
        if (find == _id_to_file_handle.end())
        {
            return false;
        }

        const auto &handles = find->second;
        for (auto iter = handles.begin(); iter != handles.end(); ++iter)
        {
            if (iter->get()->has_write_mode())
            {
                return true;
            }
        }

        return false;
    }

    uint64_t VirtualFileSystem::make_id(uint32_t entry_id, IFileSystem *fs) const
    {
        return static_cast<uint64_t>(fs->filesystem_id()) << 32lu | static_cast<uint64_t>(entry_id);
    }
}
