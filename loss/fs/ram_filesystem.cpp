#include "ram_filesystem.h"

namespace loss
{
    RamFileSystem::RamFileSystem() :
        _root(1),
        _id_counter(1)
    {
        _entry_index[1] = &_root;
    }

    IOResult RamFileSystem::read(uint32_t file_id, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (file_id == 0 || buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (count == 0)
        {
            return IOResult(0, SUCCESS);
        }

        auto find = _entry_index.find(file_id);
        if (find == _entry_index.end())
        {
            return IOResult(0, ENTRY_NOT_FOUND);
        }

        auto file = dynamic_cast<File *>(find->second);
        if (file == nullptr)
        {
            return IOResult(0, WRONG_ENTRY_TYPE);
        }

        return file->read(offset, count, buffer);
    }

    IOResult RamFileSystem::write(uint32_t file_id, uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (file_id == 0 || data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }
        if (count == 0)
        {
            return IOResult(0, SUCCESS);
        }

        auto find = _entry_index.find(file_id);
        if (find == _entry_index.end())
        {
            return IOResult(0, ENTRY_NOT_FOUND);
        }

        auto file = dynamic_cast<File *>(find->second);
        if (file == nullptr)
        {
            return IOResult(0, WRONG_ENTRY_TYPE);
        }

        return file->write(offset, count, data);
    }

    ReturnCode RamFileSystem::create_file(uint32_t folder_id, const std::string &name)
    {
        return add_entry(folder_id, name, new_file(folder_id));
    }
    ReturnCode RamFileSystem::create_folder(uint32_t folder_id, const std::string &name)
    {
        return add_entry(folder_id, name, new_folder(folder_id));
    }

    ReturnCode RamFileSystem::add_entry(uint32_t folder_id, const std::string &name, Entry *entry)
    {
        if (folder_id == 0 || name.size() == 0)
        {
            return NULL_PARAMETER;
        }

        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        auto folder = dynamic_cast<Folder *>(find->second);
        if (folder == nullptr)
        {
            return WRONG_ENTRY_TYPE;
        }

        return folder->add_entry(name, entry);

    }
    ReturnCode RamFileSystem::read_folder(uint32_t folder_id, FolderEntry *to_populate)
    {
        if (to_populate == nullptr)
        {
            return NULL_PARAMETER;
        }

        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }
        
        auto ram_folder = dynamic_cast<Folder *>(find->second);
        if (ram_folder == nullptr)
        {
            return WRONG_ENTRY_TYPE;
        }

        // Populate folders
        for (auto iter : *ram_folder)
        {
            auto file = dynamic_cast<File *>(iter.second);
            if (file != nullptr)
            {
                auto entry = new FileEntry(folder_id, this);
                entry->size(file->size());
                entry->id(file->id());
                auto result = to_populate->add_file(iter.first, entry);
                if (result != SUCCESS)
                {
                    return result;
                }

                continue;
            }

            auto folder = dynamic_cast<Folder *>(iter.second);
            if (folder != nullptr)
            {
                auto entry = new FolderEntry(folder_id, this);
                entry->id(iter.second->id());
                auto result = to_populate->add_folder(iter.first, entry);
                if (result != SUCCESS)
                {
                    return result;
                }

                continue;
            }

            auto mount_point = dynamic_cast<MountPoint *>(iter.second);
            if (mount_point != nullptr)
            {
                auto entry = new FolderEntry(folder_id, mount_point->fs());
                entry->id(iter.second->id());
                auto result = to_populate->add_folder(iter.first, entry);
                if (result != SUCCESS)
                {
                    return result;
                }

                continue;
            }
        }

        return SUCCESS;
    }

    FindFolderResult RamFileSystem::find_folder(uint32_t folder_id, const std::string &name)
    {
        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return FindFolderResult(0, ENTRY_NOT_FOUND, this);
        }

        auto parent_folder = dynamic_cast<Folder *>(find->second);
        if (parent_folder == nullptr)
        {
            return FindFolderResult(0, WRONG_ENTRY_TYPE, this);
        }

        Entry *entry = nullptr;
        auto status = parent_folder->find_entry(name, &entry);
        if (status != SUCCESS)
        {
            return FindFolderResult(0, status, this);
        }

        auto folder = dynamic_cast<Folder *>(entry);
        if (folder != nullptr)
        {
            return FindFolderResult(entry->id(), status, this);
        }

        auto mount_point = dynamic_cast<MountPoint *>(entry);
        if (mount_point != nullptr)
        {
            return FindFolderResult(entry->id(), status, mount_point->fs());
        }

        return FindFolderResult(0, WRONG_ENTRY_TYPE, this);
    }
            
    ReturnCode RamFileSystem::mount(uint32_t folder_id, const std::string &name, IFileSystem *fs)
    {
        if (name.size() == 0 || fs == nullptr)
        {
            return NULL_PARAMETER;
        }

        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        auto folder = dynamic_cast<Folder *>(find->second);
        if (folder == nullptr)
        {
            return WRONG_ENTRY_TYPE;
        }

        auto id = next_id();
        auto result = new MountPoint(id, fs);
        _entry_index[id] = result;
        return folder->add_entry(name, result);
    }

    ReturnCode RamFileSystem::remove_entry(uint32_t entry_id)
    {
        if (entry_id == 0)
        {
            return NULL_PARAMETER;
        }

        // Cannot remove root
        if (entry_id == 1)
        {
            return WRONG_ENTRY_TYPE;
        }

        auto find = _entry_index.find(entry_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        auto folder = dynamic_cast<Folder *>(find->second);
        if (folder != nullptr)
        {
            if (folder->size() > 0)
            {
                return FOLDER_NOT_EMPTY;
            }
        }

        auto parent_find = _entry_index.find(find->second->parent_folder_id());
        if (parent_find == _entry_index.end())
        {
            // Parent not found?
            return ENTRY_NOT_FOUND;
        }

        auto parent_folder = dynamic_cast<Folder *>(parent_find->second);
        if (parent_folder == nullptr)
        {
            // Parent wrong entry type?
            return WRONG_ENTRY_TYPE;
        }

        auto result = parent_folder->remove_entry(entry_id);
        if (result == SUCCESS)
        {
            _entry_index.erase(find);
        }

        return result;
    }

    RamFileSystem::Entry::Entry(uint32_t id) :
        _id(id),
        _parent_folder_id(0)
    {

    }
    RamFileSystem::Entry::~Entry()
    {

    }
    uint32_t RamFileSystem::Entry::id() const
    {
        return _id;
    }
                    
    void RamFileSystem::Entry::parent_folder_id(uint32_t id)
    {
        _parent_folder_id = id;
    }
    uint32_t RamFileSystem::Entry::parent_folder_id() const
    {
        return _parent_folder_id;
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
    ReturnCode RamFileSystem::Folder::add_entry(const std::string &name, RamFileSystem::Entry *entry)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _entries[name] = entry;
        return SUCCESS;
    }
    ReturnCode RamFileSystem::Folder::remove_entry(const std::string &name)
    {
        auto find = _entries.find(name);
        if (find == _entries.end())
        {
            return ENTRY_NOT_FOUND;
        }
        _entries.erase(find);
        return SUCCESS;
    }
    ReturnCode RamFileSystem::Folder::remove_entry(uint32_t id)
    {
        for (auto iter = _entries.begin(); iter != _entries.end(); ++iter)
        {
            if (iter->second->id() == id)
            {
                _entries.erase(iter);
                return SUCCESS;
            }
        }
        return ENTRY_NOT_FOUND;
    }
    ReturnCode RamFileSystem::Folder::find_entry(const std::string &name, RamFileSystem::Entry **entry) const
    {
        auto find = _entries.find(name);
        if (find == _entries.end())
        {
            return ENTRY_NOT_FOUND;
        }

        *entry = find->second;
        return SUCCESS;
    }

    bool RamFileSystem::Folder::has_entry(const std::string &name) const
    {
        auto find = _entries.find(name);
        if (find != _entries.end())
        {
            return true;
        }
        return false;
    }

    RamFileSystem::Folder::EntryMap::const_iterator RamFileSystem::Folder::begin() const
    {
        return _entries.begin();
    }
    RamFileSystem::Folder::EntryMap::const_iterator RamFileSystem::Folder::end() const
    {
        return _entries.end();
    }
    uint32_t RamFileSystem::Folder::size() const
    {
        return static_cast<uint32_t>(_entries.size());
    }

    RamFileSystem::MountPoint::MountPoint(uint32_t id, IFileSystem *fs) :
        Entry(id),
        _fs(fs)
    {

    }
    IFileSystem *RamFileSystem::MountPoint::fs() const
    {
        return _fs;
    }

    uint32_t RamFileSystem::next_id()
    {
        return ++_id_counter;
    }

    RamFileSystem::Folder *RamFileSystem::new_folder(uint32_t parent_id)
    {
        auto id = next_id();
        auto result = new Folder(id);
        result->parent_folder_id(parent_id);
        _entry_index[id] = result;
        return result;
    }
    RamFileSystem::DataFile *RamFileSystem::new_file(uint32_t parent_id)
    {
        auto id = next_id();
        auto result = new DataFile(id);
        result->parent_folder_id(parent_id);
        _entry_index[id] = result;
        return result;
    }
}
