#include "ram_filesystem.h"

namespace loss
{
    RamFileSystem::RamFileSystem() :
        _id_counter(ROOT_ID)
    {
        _root = new Folder(ROOT_ID);
        _entry_index[ROOT_ID] = std::unique_ptr<Entry>(_root);
    }

    IOResult RamFileSystem::read(uint32_t file_id, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (file_id == NULL_ID || buffer == nullptr)
        {
            return IOResult(NULL_ID, NULL_PARAMETER);
        }
        if (count == 0)
        {
            return IOResult(NULL_ID, SUCCESS);
        }

        auto find = _entry_index.find(file_id);
        if (find == _entry_index.end())
        {
            return IOResult(NULL_ID, ENTRY_NOT_FOUND);
        }

        auto file = dynamic_cast<File *>(find->second.get());
        if (file == nullptr)
        {
            return IOResult(NULL_ID, WRONG_ENTRY_TYPE);
        }

        return file->read(offset, count, buffer);
    }

    IOResult RamFileSystem::write(uint32_t file_id, uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (file_id == NULL_ID || data == nullptr)
        {
            return IOResult(NULL_ID, NULL_PARAMETER);
        }
        if (count == 0)
        {
            return IOResult(NULL_ID, SUCCESS);
        }

        auto find = _entry_index.find(file_id);
        if (find == _entry_index.end())
        {
            return IOResult(NULL_ID, ENTRY_NOT_FOUND);
        }

        auto file = dynamic_cast<File *>(find->second.get());
        if (file == nullptr)
        {
            return IOResult(NULL_ID, WRONG_ENTRY_TYPE);
        }

        return file->write(offset, count, data);
    }

    CreateEntryResult RamFileSystem::create_file(uint32_t folder_id, const std::string &name)
    {
        return add_entry(folder_id, name, new_file(folder_id));
    }
    CreateEntryResult RamFileSystem::create_folder(uint32_t folder_id, const std::string &name)
    {
        return add_entry(folder_id, name, new_folder(folder_id));
    }

    CreateEntryResult RamFileSystem::add_entry(uint32_t folder_id, const std::string &name, Entry *entry)
    {
        if (folder_id == NULL_ID || name.size() == 0)
        {
            return CreateEntryResult(NULL_ID, NULL_PARAMETER);
        }

        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return CreateEntryResult(NULL_ID, ENTRY_NOT_FOUND);
        }

        auto folder = dynamic_cast<Folder *>(find->second.get());
        if (folder == nullptr)
        {
            return CreateEntryResult(NULL_ID, WRONG_ENTRY_TYPE);
        }

        return CreateEntryResult(entry->id(), folder->add_entry(name, entry));

    }
    ReturnCode RamFileSystem::read_folder(uint32_t folder_id, FolderEntry &to_populate)
    {
        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }
        
        auto ram_folder = dynamic_cast<Folder *>(find->second.get());
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
                auto result = to_populate.add_file(iter.first, entry);
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
                auto result = to_populate.add_folder(iter.first, entry);
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
                auto result = to_populate.add_folder(iter.first, entry);
                if (result != SUCCESS)
                {
                    return result;
                }

                continue;
            }
        }

        return SUCCESS;
    }

    FindEntryResult RamFileSystem::find_entry(uint32_t folder_id, const std::string &name)
    {
        Entry *entry = nullptr;
        if (folder_id == ROOT_ID)
        {
            if (name.empty())
            {
                return FindEntryResult(ROOT_ID, SUCCESS, this);
            }
            auto status = _root->find_entry(name, &entry);
            if (status != SUCCESS)
            {
                return FindEntryResult(NULL_ID, status, this);
            }
        }
        else
        {
            auto find = _entry_index.find(folder_id);
            if (find == _entry_index.end())
            {
                return FindEntryResult(NULL_ID, ENTRY_NOT_FOUND, this);
            }

            auto parent_folder = dynamic_cast<Folder *>(find->second.get());
            if (parent_folder == nullptr)
            {
                return FindEntryResult(NULL_ID, WRONG_ENTRY_TYPE, this);
            }

            auto status = parent_folder->find_entry(name, &entry);
            if (status != SUCCESS)
            {
                return FindEntryResult(NULL_ID, status, this);
            }
        }

        auto mount_point = dynamic_cast<MountPoint *>(entry);
        if (mount_point != nullptr)
        {
            return FindEntryResult(entry->id(), SUCCESS, mount_point->fs());
        }

        return FindEntryResult(entry->id(), SUCCESS, this);
    }
            
    CreateEntryResult RamFileSystem::mount(uint32_t folder_id, const std::string &name, IFileSystem *fs)
    {
        if (name.size() == 0 || fs == nullptr)
        {
            return CreateEntryResult(NULL_ID, NULL_PARAMETER);
        }

        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return CreateEntryResult(NULL_ID, ENTRY_NOT_FOUND);
        }

        auto folder = dynamic_cast<Folder *>(find->second.get());
        if (folder == nullptr)
        {
            return CreateEntryResult(NULL_ID, WRONG_ENTRY_TYPE);
        }

        auto id = next_id();
        auto result = new MountPoint(id, fs);
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return CreateEntryResult(id, folder->add_entry(name, result));
    }

    ReturnCode RamFileSystem::entry_size(uint32_t entry_id, uint32_t &size)
    {
        if (entry_id == NULL_ID)
        {
            return NULL_PARAMETER;
        }

        auto find = _entry_index.find(entry_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        auto file = dynamic_cast<File *>(find->second.get());
        if (file != nullptr)
        {
            size = file->size();
            return SUCCESS;
        }

        auto folder = dynamic_cast<Folder *>(find->second.get());
        if (folder != nullptr)
        {
            size = folder->size();
            return SUCCESS;
        }

        return WRONG_ENTRY_TYPE;
    }
    ReturnCode RamFileSystem::entry_metadata(uint32_t entry_id, MetadataDef &metadata)
    {
        if (entry_id == NULL_ID)
        {
            return NULL_PARAMETER;
        }

        auto find = _entry_index.find(entry_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        metadata = find->second->metadata();
        return SUCCESS;
    }
    ReturnCode RamFileSystem::update_entry_metadata(uint32_t entry_id, const MetadataDef &metadata)
    {
        if (entry_id == NULL_ID)
        {
            return NULL_PARAMETER;
        }

        auto find = _entry_index.find(entry_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        find->second->metadata(metadata);
        return SUCCESS;
    }

    ReturnCode RamFileSystem::remove_entry(uint32_t entry_id)
    {
        if (entry_id == NULL_ID)
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

        auto folder = dynamic_cast<Folder *>(find->second.get());
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

        auto parent_folder = dynamic_cast<Folder *>(parent_find->second.get());
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
        _parent_folder_id(NULL_ID)
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
    
    void RamFileSystem::Entry::metadata(const MetadataDef &metadata)
    {
        _metadata = metadata;
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
            return IOResult(NULL_ID, NULL_PARAMETER);
        }

        if (offset >= _data.size() || count == 0)
        {
            return IOResult(NULL_ID, SUCCESS);
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
            return IOResult(NULL_ID, NULL_PARAMETER);
        }
        if (offset > _data.size())
        {
            return IOResult(NULL_ID, OUT_OF_BOUNDS);
        }
        if (count == 0)
        {
            return IOResult(NULL_ID, SUCCESS);
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
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return result;
    }
    RamFileSystem::DataFile *RamFileSystem::new_file(uint32_t parent_id)
    {
        auto id = next_id();
        auto result = new DataFile(id);
        result->parent_folder_id(parent_id);
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return result;
    }
}
