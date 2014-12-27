#include "ram_filesystem.h"

namespace loss
{
    RamFileSystem::RamFileSystem() :
        _root(1),
        _id_counter(1)
    {
        _folder_index[1] = &_root;
    }

    IOResult RamFileSystem::read(FolderEntry *folder, const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (folder == nullptr || name.size() == 0 || buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (count == 0)
        {
            return IOResult(0, SUCCESS);
        }

        auto find = _folder_index.find(folder->id());  
        if (find == _folder_index.end())
        {
            return IOResult(0, ENTRY_NOT_FOUND);
        }

        File *file = nullptr;
        auto result = find->second->find_file(name, &file);
        if (result != SUCCESS)
        {
            return IOResult(0, result);
        }

        return file->read(offset, count, buffer);
    }
    IOResult RamFileSystem::write(FolderEntry *folder, const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data)
    {
/*
 *        Path path(name);
 *        path.dir_to_filename();
 *
 *        auto folder = &_root;
 *        for (auto part : path.dirs())
 *        {
 *            ReturnCode result = folder->find_folder(part, &folder);
 *            if (result != SUCCESS)
 *            {
 *                return IOResult(0, result);
 *            }
 *        }
 *
 *        File *file = nullptr;
 *        ReturnCode result = folder->find_file(path.filename(), &file);
 *        if (result == ENTRY_NOT_FOUND)
 *        {
 *            file = new DataFile();
 *            folder->add_file(path.filename(), file);
 *        }
 *        else if (result != SUCCESS)
 *        {
 *            return IOResult(0, result);
 *        }
 *
 *        return file->write(offset, count, data);
 */
        if (folder == nullptr || name.size() == 0 || data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (count == 0)
        {
            return IOResult(0, SUCCESS);
        }

        auto find = _folder_index.find(folder->id());  
        if (find == _folder_index.end())
        {
            return IOResult(0, ENTRY_NOT_FOUND);
        }

        File *file = nullptr;
        auto result = find->second->find_file(name, &file);
        if (result != SUCCESS)
        {
            return IOResult(0, result);
        }

        return file->write(offset, count, data);
    }

    ReturnCode RamFileSystem::create_folder(FolderEntry *folder, const std::string &name)
    {
        if (folder == nullptr || name.size() == 0)
        {
            return NULL_PARAMETER;
        }

        auto find = _folder_index.find(folder->id());
        if (find == _folder_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        return find->second->add_folder(name, new_folder());
        /*
        Path path(name);
        path.dir_to_filename();
        
        auto folder = &_root;
        for (auto part : path.dirs())
        {
            ReturnCode result = folder->find_folder(part, &folder);
            if (result != SUCCESS)
            {
                return result;
            }
        }

        return folder->add_folder(path.filename(), new Folder());
        */
    }
    ReturnCode RamFileSystem::read_folder(FolderEntry *folder, const std::string &name, FolderEntry *to_populate)
    {
        if (to_populate == nullptr || name.size() == 0 || to_populate == nullptr)
        {
            return NULL_PARAMETER;
        }

        auto ram_folder = &_root;
        auto id = ram_folder->id();
        if (folder != nullptr)
        {
            id = folder->id();
            auto find = _folder_index.find(id);
            if (find == _folder_index.end())
            {
                return ENTRY_NOT_FOUND;
            }
            ram_folder = find->second;
        }

        // Populate folders
        for (auto iter = ram_folder->begin_folders(); iter != ram_folder->end_folders(); ++iter)
        {
            auto entry = new FolderEntry(this);
            entry->id(iter->second->id());
            auto result = to_populate->add_folder(iter->first, entry);
            if (result != SUCCESS)
            {
                return result;
            }
        }

        // Populate files
        for (auto iter = ram_folder->begin_files(); iter != ram_folder->end_files(); ++iter)
        {
            auto entry = new FileEntry(this);
            entry->size(iter->second->size());
            entry->id(iter->second->id());
            auto result = to_populate->add_file(iter->first, entry);
            if (result != SUCCESS)
            {
                return result;
            }
        }
        return SUCCESS;
    }

    FindFolderResult RamFileSystem::find_folder(uint32_t id, const std::string &name)
    {
        return FindFolderResult(0, SUCCESS, this);
    }

    RamFileSystem::Entry::Entry(uint32_t id) :
        _id(id)
    {

    }
    uint32_t RamFileSystem::Entry::id() const
    {
        return _id;
    }

    MetadataDef &RamFileSystem::Entry::metadata()
    {
        return _metadata;
    }
    const MetadataDef &RamFileSystem::Entry::metadata() const
    {
        return _metadata;
    }

    RamFileSystem::File::File(uint32_t id) :
        Entry(id)
    {

    }

    RamFileSystem::DataFile::DataFile(uint32_t id) :
        File(id)
    {

    }
    uint32_t RamFileSystem::DataFile::size() const
    {
        return static_cast<uint32_t>(_data.size());
    }
    IOResult RamFileSystem::DataFile::read(uint32_t offset, uint32_t count, uint8_t *buffer)
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
            read_count -= (end_index - _data.size());
            end_index = _data.size();
        }

        for (uint32_t i = offset, j = 0; i < end_index; i++, j++)
        {
            buffer[j] = _data[i];
        }
        return IOResult(read_count, SUCCESS);
    }
    IOResult RamFileSystem::DataFile::write(uint32_t offset, uint32_t count, const uint8_t *data)
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

        if (offset + count > _data.size())
        {
            _data.resize(offset + count, 0);
        }
        for (uint32_t i = 0, j = offset; i < count; i++, j++)
        {
            _data[j] = data[i];
        }
        return IOResult(count, SUCCESS);
    }
    
    RamFileSystem::Folder::Folder(uint32_t id) :
        Entry(id)
    {

    }
    ReturnCode RamFileSystem::Folder::add_file(const std::string &name, RamFileSystem::File *file)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _files[name] = file;
        return SUCCESS;
    }
    ReturnCode RamFileSystem::Folder::find_file(const std::string &name, RamFileSystem::File **file) const
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

        *file = find->second;
        return SUCCESS;
    }

    ReturnCode RamFileSystem::Folder::add_folder(const std::string &name, RamFileSystem::Folder *folder)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _folders[name] = folder;
        return SUCCESS;
    }
    ReturnCode RamFileSystem::Folder::find_folder(const std::string &name, RamFileSystem::Folder **folder) const
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

        *folder = find->second;
        return SUCCESS;
    }

    ReturnCode RamFileSystem::Folder::find_entry(const std::string &name, RamFileSystem::Entry *entry) const
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
    bool RamFileSystem::Folder::has_entry(const std::string &name) const
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

    RamFileSystem::Folder::FileMap::const_iterator RamFileSystem::Folder::begin_files() const
    {
        return _files.begin();
    }
    RamFileSystem::Folder::FileMap::const_iterator RamFileSystem::Folder::end_files() const
    {
        return _files.end();
    }
    uint32_t RamFileSystem::Folder::num_files() const
    {
        return static_cast<uint32_t>(_files.size());
    }

    RamFileSystem::Folder::FolderMap::const_iterator RamFileSystem::Folder::begin_folders() const
    {
        return _folders.begin();
    }
    RamFileSystem::Folder::FolderMap::const_iterator RamFileSystem::Folder::end_folders() const
    {
        return _folders.end();
    }
    uint32_t RamFileSystem::Folder::num_folders() const
    {
        return static_cast<uint32_t>(_folders.size());
    }

    uint32_t RamFileSystem::next_id()
    {
        return ++_id_counter;
    }

    RamFileSystem::Folder *RamFileSystem::new_folder()
    {
        auto id = next_id();
        auto result = new Folder(id);
        _folder_index[id] = result;
        return result;
    }
    RamFileSystem::DataFile *RamFileSystem::new_file()
    {
        auto id = next_id();
        auto result = new DataFile(id);
        _file_index[id] = result;
        return result;
    }
}
