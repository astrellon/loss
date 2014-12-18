#include "virtual_filesystem.h"

#include "path.h"

namespace loss
{
    IOResult VirtualFileSystem::read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        Path path(name);
        path.dir_to_filename();

        auto folder = &_root;
        for (auto part : path.dirs())
        {
            ReturnCode result = folder->find_folder(part, folder);
            if (result != SUCCESS)
            {
                return IOResult(0, result);
            }
        }

        File *file = nullptr;
        ReturnCode result = folder->find_file(path.filename(), file);
        if (result != SUCCESS)
        {
            return IOResult(0, result);
        }

        return file->read(offset, count, buffer);
        /*
        uint32_t j = 0, i = offset;
        for (; j < count && i < file->.size(); i++, j++)
        {
            buffer[j] = test[i];
        }
        if (j < count)
        {
            buffer[j++] = '\0';
        }

        return IOResult(j, SUCCESS);
        */
    }
    IOResult VirtualFileSystem::write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data)
    {
        Path path(name);
        path.dir_to_filename();

        auto folder = &_root;
        for (auto part : path.dirs())
        {
            ReturnCode result = folder->find_folder(part, folder);
            if (result != SUCCESS)
            {
                return IOResult(0, result);
            }
        }

        File *file = nullptr;
        ReturnCode result = folder->find_file(path.filename(), file);
        if (result == ENTRY_NOT_FOUND)
        {
            file = new DataFile();
            folder->add_file(path.filename(), file);
        }
        else if (result != SUCCESS)
        {
            return IOResult(0, result);
        }

        return file->write(offset, count, data);
    }

    ReturnCode VirtualFileSystem::getdir(const std::string &name, FolderEntry *to_populate)
    {
        Path path(name);
        path.filename_to_dir();

        auto folder = &_root;
        for (auto part : path.dirs())
        {
            ReturnCode result = folder->find_folder(part, folder);
            if (result != SUCCESS)
            {
                return result;
            }
        }

        // Populate folders
        for (auto iter = folder->begin_folders(); iter != folder->end_folders(); ++iter)
        {
            auto entry = new FolderEntry();
            auto result = to_populate->add_folder(iter->first, entry);
            if (result != SUCCESS)
            {
                return result;
            }
        }

        // Populate files
        for (auto iter = folder->begin_files(); iter != folder->end_files(); ++iter)
        {
            auto entry = new FileEntry();
            entry->size(iter->second->size());
            auto result = to_populate->add_file(iter->first, entry);
            if (result != SUCCESS)
            {
                return result;
            }
        }
        return SUCCESS;
    }

    MetadataDef &VirtualFileSystem::Entry::metadata()
    {
        return _metadata;
    }
    const MetadataDef &VirtualFileSystem::Entry::metadata() const
    {
        return _metadata;
    }

    uint32_t VirtualFileSystem::DataFile::size() const
    {
        return 0;
    }
    IOResult VirtualFileSystem::DataFile::read(uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        if (offset >= _data.size() || count == 0)
        {
            return IOResult(0, SUCCESS);
        }
        uint32_t end_index = offset + count;
        uint32_t read_count = count;
        if (end_index > _data.size())
        {
            read_count -= (_data.size() - end_index);
            end_index = _data.size();
        }

        for (uint32_t i = offset, j = 0; i < end_index; i++, j++)
        {
            buffer[j] = _data[i];
        }
        return IOResult(read_count, SUCCESS);
    }
    IOResult VirtualFileSystem::DataFile::write(uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (offset > _data.size())
        {
            return IOResult(0, OUT_OF_BOUNDS);
        }
        if (count == 0)
        {
            return IOResult(0, SUCCESS);
        }

        auto iter = _data.begin() + offset;
        for (uint32_t i = 0; i < count; i++, ++iter)
        {
            _data.insert(iter, data[i]);
        }
        return IOResult(count, SUCCESS);
    }
    
    ReturnCode VirtualFileSystem::Folder::add_file(const std::string &name, VirtualFileSystem::File *file)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _files[name] = file;
        return SUCCESS;
    }
    ReturnCode VirtualFileSystem::Folder::find_file(const std::string &name, VirtualFileSystem::File *file) const
    {
        auto find = _files.find(name);
        if (find == _files.end())
        {
            auto find_folder = _folders.find(name);
            if (find_folder != _folders.end())
            {
                return WRONG_ENTRY_TYPE;
            }
            return ENTRY_NOT_FOUND;
        }

        file = find->second;
        return SUCCESS;
    }

    ReturnCode VirtualFileSystem::Folder::add_folder(const std::string &name, VirtualFileSystem::Folder *folder)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _folders[name] = folder;
        return SUCCESS;
    }
    ReturnCode VirtualFileSystem::Folder::find_folder(const std::string &name, VirtualFileSystem::Folder *folder) const
    {
        auto find = _folders.find(name);
        if (find == _folders.end())
        {
            auto find_file = _files.find(name);
            if (find_file != _files.end())
            {
                return WRONG_ENTRY_TYPE;
            }
            return ENTRY_NOT_FOUND;
        }

        folder = find->second;
        return SUCCESS;
    }

    ReturnCode VirtualFileSystem::Folder::find_entry(const std::string &name, VirtualFileSystem::Entry *entry) const
    {
        auto find = _files.find(name);
        if (find == _files.end())
        {
            auto find_folder = _folders.find(name);
            if (find_folder != _folders.end())
            {
                entry = find_folder->second;
                return SUCCESS;
            }
            return ENTRY_NOT_FOUND;
        }

        entry = find->second;
        return SUCCESS;
    }
    bool VirtualFileSystem::Folder::has_entry(const std::string &name) const
    {
        auto find = _files.find(name);
        if (find != _files.end())
        {
            return true;
        }
        auto find_folder = _folders.find(name);
        if (find_folder != _folders.end())
        {
            return true;
        }
        return false;
    }

    VirtualFileSystem::Folder::FileMap::const_iterator VirtualFileSystem::Folder::begin_files() const
    {
        return _files.begin();
    }
    VirtualFileSystem::Folder::FileMap::const_iterator VirtualFileSystem::Folder::end_files() const
    {
        return _files.end();
    }
    uint32_t VirtualFileSystem::Folder::num_files() const
    {
        return static_cast<uint32_t>(_files.size());
    }

    VirtualFileSystem::Folder::FolderMap::const_iterator VirtualFileSystem::Folder::begin_folders() const
    {
        return _folders.begin();
    }
    VirtualFileSystem::Folder::FolderMap::const_iterator VirtualFileSystem::Folder::end_folders() const
    {
        return _folders.end();
    }
    uint32_t VirtualFileSystem::Folder::num_folders() const
    {
        return static_cast<uint32_t>(_folders.size());
    }
}
