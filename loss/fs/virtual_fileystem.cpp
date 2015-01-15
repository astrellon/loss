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

        if (open_mode | FileHandle::WRITE && find_write_handle(process_id, find.id(), find.fs()))
        {
            return FILE_HAS_WRITE_LOCK;
        }

        handle = new FileHandle(find.id(), process_id, open_mode, result.fs());
        _process_file_handles[process_id].push_back(std::unique_ptr<FileHandle>(handle)); 
        return SUCCESS;
    }
    ReturnCode VirtualFileSystem::close(FileHandle *handle)
    {
        if (handle == nullptr)
        {
            return NULL_PARAMETER;
        }

        const auto find_process = _process_file_handles.find(handle->process_id());
        if (find_process == _process_file_handles.end())
        {
            return CANNOT_FIND_PROCESS;
        }

        auto &handles = find_process->second;
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
    ReturnCode VirtualFileSystem::entry_size(IEntry *entry, uint32_t &size)
    {
        if (entry == nullptr)
        {
            return NULL_PARAMETER;
        }
        return entry->filesystem()->entry_size(entry->id(), size);
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
    ReturnCode VirtualFileSystem::entry_metadata(IEntry *entry, MetadataDef &metadata)
    {
        if (entry == nullptr)
        {
            return NULL_PARAMETER;
        }
        return entry->filesystem()->entry_metadata(entry->id(), metadata);
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
    ReturnCode VirtualFileSystem::update_entry_metadata(IEntry *entry, const MetadataDef &metadata)
    {
        if (entry == nullptr)
        {
            return NULL_PARAMETER;
        }
        return entry->filesystem()->update_entry_metadata(entry->id(), metadata);
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
    ReturnCode VirtualFileSystem::remove_entry(IEntry *entry)
    {
        if (entry == nullptr)
        {
            return NULL_PARAMETER;
        }

        return entry->filesystem()->remove_entry(entry->id());
    }

    FindEntryResult VirtualFileSystem::follow_path(const Path &path, uint32_t folder_id)
    {
        IFileSystem *fs = _root_filesystem;
        for (auto dir : path.dirs())
        {
            auto result = fs->find_entry(folder_id, dir);
            if (result.status() != SUCCESS)
            {
                return result;
            }
            fs = result.fs();
            folder_id = result.id();
        }

        return FindEntryResult(folder_id, SUCCESS, fs);
    }

    bool VirtualFileSystem::find_write_handle(uint32_t process_id, uint32_t entry_id, IFileSystem *fs) const
    {
        const auto find = _process_file_handles.find(process_id);
        if (find == _process_file_handles.end())
        {
            return false;
        }

        const auto &handles = find->second;
        for (auto iter = handles.begin(); iter != handles.end(); ++iter)
        {
            const auto handle = iter->get();
            if (handle->entry_id() == entry_id && 
                    handle->filesystem() == fs && 
                    handle->has_write_mode())
            {
                return true;
            }
        }

        return false;
    }
}
