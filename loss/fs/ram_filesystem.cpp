#include "ram_filesystem.h"

namespace loss
{
    // RamFileSystem {{{
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

        auto type = find->second->metadata().type();
        if (type == FILE_ENTRY)
        {
            auto file = dynamic_cast<File *>(find->second.get());
            if (file != nullptr)
            {
                return file->read(offset, count, buffer);
            }
        }
        else if (type == CHARACTER_DEVICE_ENTRY)
        {
            auto char_device = dynamic_cast<CharacterDevice *>(find->second.get());
            if (char_device != nullptr)
            {
                return char_device->read(offset, count, buffer);
            }
        }

        return IOResult(NULL_ID, WRONG_ENTRY_TYPE);
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

        auto type = find->second->metadata().type();
        if (type == FILE_ENTRY)
        {
            auto file = dynamic_cast<File *>(find->second.get());
            if (file != nullptr)
            {
                return file->write(offset, count, data);
            }
        }
        else if (type == CHARACTER_DEVICE_ENTRY)
        {
            auto char_device = dynamic_cast<CharacterDevice *>(find->second.get());
            if (char_device != nullptr)
            {
                return char_device->write(offset, count, data);
            }
        }

        return IOResult(NULL_ID, WRONG_ENTRY_TYPE);
    }

    CreateEntryResult RamFileSystem::create_file(uint32_t folder_id, const std::string &name)
    {
        return add_entry(folder_id, name, new_file(folder_id));
    }
    CreateEntryResult RamFileSystem::create_symlink(uint32_t folder_id, const std::string &name, const std::string &link)
    {
        return add_entry(folder_id, name, new_symlink(folder_id, link));
    }
    CreateEntryResult RamFileSystem::create_folder(uint32_t folder_id, const std::string &name)
    {
        return add_entry(folder_id, name, new_folder(folder_id));
    }
    CreateEntryResult RamFileSystem::create_char_device(uint32_t folder_id, const std::string &name, ICharacterDevice *device)
    {
        return add_entry(folder_id, name, new_char_device(folder_id, device));
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
            std::cout << "Failed to add entry: " << name << " with " << folder_id << "\n";
            return CreateEntryResult(NULL_ID, ENTRY_NOT_FOUND);
        }

        if (find->second->metadata().type() != FOLDER_ENTRY)
        {
            return CreateEntryResult(NULL_ID, WRONG_ENTRY_TYPE);
        }

        auto folder = dynamic_cast<Folder *>(find->second.get());
        if (folder == nullptr)
        {
            return CreateEntryResult(NULL_ID, INTERNAL_ERROR);
        }

        return CreateEntryResult(entry->id(), folder->add_entry(name, entry));

    }
    ReturnCode RamFileSystem::read_folder(uint32_t proess_id, uint32_t folder_id, FolderEntry &to_populate)
    {
        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }
        
        if (find->second->metadata().type() != FOLDER_ENTRY)
        {
            return WRONG_ENTRY_TYPE;
        }

        auto ram_folder = dynamic_cast<Folder *>(find->second.get());
        if (ram_folder == nullptr)
        {
            return INTERNAL_ERROR;
        }

        // Populate folders
        for (auto iter : *ram_folder)
        {
            auto type = iter.second->metadata().type();

            // TODO See if dynamic_cast-ing is better or to stick a virtual method on
            // entry to figure out the type.
            if (type == FILE_ENTRY)
            {
                auto file = dynamic_cast<File *>(iter.second);
                if (file != nullptr)
                {
                    auto entry = new FileEntry(folder_id, this);
                    entry->size(file->size());
                    entry->id(file->id());
                    auto result = to_populate.add_entry(iter.first, entry);
                    if (result != SUCCESS)
                    {
                        return result;
                    }

                    continue;
                }
            }

            if (type == FOLDER_ENTRY)
            {
                auto folder = dynamic_cast<Folder *>(iter.second);
                if (folder != nullptr)
                {
                    auto entry = new FolderEntry(folder_id, this);
                    entry->id(iter.second->id());
                    auto result = to_populate.add_entry(iter.first, entry);
                    if (result != SUCCESS)
                    {
                        return result;
                    }

                    continue;
                }
            }

            if (type == SYMLINK_ENTRY)
            {
                auto symlink = dynamic_cast<Symlink *>(iter.second);
                if (symlink != nullptr)
                {
                    auto entry = new SymlinkEntry(folder_id, this, symlink->link());
                    auto result = to_populate.add_entry(iter.first, entry);
                    if (result != SUCCESS)
                    {
                        return result;
                    }
                    continue;
                }
            }

            if (type == MOUNT_POINT_ENTRY)
            {
                auto mount_point = dynamic_cast<MountPoint *>(iter.second);
                if (mount_point != nullptr)
                {
                    auto entry = new FolderEntry(folder_id, mount_point->fs());
                    entry->id(iter.second->id());
                    auto result = to_populate.add_entry(iter.first, entry);
                    if (result != SUCCESS)
                    {
                        return result;
                    }

                    continue;
                }
            }

            if (type == CHARACTER_DEVICE_ENTRY)
            {
                auto char_device = dynamic_cast<CharacterDevice *>(iter.second);
                if (char_device != nullptr)
                {
                    auto entry = new CharacterDeviceEntry(folder_id, this, char_device->device());
                    entry->id(iter.second->id());
                    auto result = to_populate.add_entry(iter.first, entry);
                    if (result != SUCCESS)
                    {
                        return result;
                    }

                    continue;
                }
            }
        }

        return SUCCESS;
    }

    ReturnCode RamFileSystem::read_symlink(uint32_t symlink_id, std::string &link)
    {
        auto find = _entry_index.find(symlink_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }

        if (find->second->metadata().type() != SYMLINK_ENTRY)
        {
            return WRONG_ENTRY_TYPE;
        }

        auto symlink = dynamic_cast<Symlink *>(find->second.get());
        if (symlink == nullptr)
        {
            return INTERNAL_ERROR;
        }

        link = symlink->link();
        return SUCCESS;
    }

    FindEntryResult RamFileSystem::find_entry(uint32_t folder_id, const std::string &name)
    {
        Entry *entry = nullptr;
        if (folder_id == ROOT_ID)
        {
            if (name.empty())
            {
                return FindEntryResult(ROOT_ID, SUCCESS, this, _root->metadata());
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
                return FindEntryResult(NULL_ID, INTERNAL_ERROR, this);
            }

            auto status = parent_folder->find_entry(name, &entry);
            if (status != SUCCESS)
            {
                return FindEntryResult(NULL_ID, status, this);
            }
        }

        if (entry->metadata().type() == MOUNT_POINT_ENTRY)
        {
            auto mount_point = dynamic_cast<MountPoint *>(entry);
            if (mount_point != nullptr)
            {
                // Mount points always mount the root folder.
                return FindEntryResult(ROOT_ID, SUCCESS, mount_point->fs(), mount_point->metadata());
            }
        }
        if (entry->metadata().type() == SYMLINK_ENTRY)
        {
            auto link = dynamic_cast<Symlink *>(entry);
            if (link != nullptr)
            {
                return FindEntryResult(entry->id(), SUCCESS, this, entry->metadata());
            }
        }

        return FindEntryResult(entry->id(), SUCCESS, this, entry->metadata());
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

        if (find->second->metadata().type() != FOLDER_ENTRY)
        {
            return CreateEntryResult(NULL_ID, WRONG_ENTRY_TYPE);
        }
        auto folder = dynamic_cast<Folder *>(find->second.get());
        if (folder == nullptr)
        {
            return CreateEntryResult(NULL_ID, INTERNAL_ERROR);
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

        auto type = find->second->metadata().type();

        if (type == FILE_ENTRY)
        {
            auto file = dynamic_cast<File *>(find->second.get());
            if (file != nullptr)
            {
                size = file->size();
                return SUCCESS;
            }
        }

        if (type == FOLDER_ENTRY)
        {
            auto folder = dynamic_cast<Folder *>(find->second.get());
            if (folder != nullptr)
            {
                size = folder->size();
                return SUCCESS;
            }
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

    ReturnCode RamFileSystem::rename(uint32_t folder_id, const std::string &name, uint32_t new_parent_id, const std::string &new_name)
    {
        if (name.size() == 0u || new_parent_id == NULL_ID || new_name.size() == 0u)
        {
            return NULL_PARAMETER;
        }

        // We did it!
        if (folder_id == new_parent_id && name == new_name)
        {
            return SUCCESS;
        }

        auto find = _entry_index.find(folder_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }
        auto current_parent = find->second.get();
        if (current_parent->metadata().type() != FOLDER_ENTRY)
        {
            return WRONG_ENTRY_TYPE;
        }

        find = _entry_index.find(new_parent_id);
        if (find == _entry_index.end())
        {
            return ENTRY_NOT_FOUND;
        }
        auto new_parent = find->second.get();
        if (new_parent->metadata().type() != FOLDER_ENTRY)
        {
            return WRONG_ENTRY_TYPE;
        }

        auto current_parent_folder = dynamic_cast<Folder *>(current_parent);
        auto new_parent_folder = dynamic_cast<Folder *>(new_parent);

        Entry *to_move = nullptr;
        auto move_find = current_parent_folder->find_entry(name, &to_move);
        if (move_find != SUCCESS)
        {
            return move_find;
        }

        auto move_result = new_parent_folder->add_entry(new_name, to_move);
        if (move_result != SUCCESS)
        {
            return move_result;
        }
        current_parent_folder->remove_entry(name);

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

        auto type = find->second->metadata().type();

        if (type == FOLDER_ENTRY)
        {
            auto folder = dynamic_cast<Folder *>(find->second.get());
            if (folder != nullptr)
            {
                if (folder->size() > 0)
                {
                    return FOLDER_NOT_EMPTY;
                }
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
    
    RamFileSystem::Folder *RamFileSystem::new_folder(uint32_t parent_id)
    {
        auto id = next_id();
        auto result = new Folder(id);
        result->parent_folder_id(parent_id);
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return result;
    }
    RamFileSystem::File *RamFileSystem::new_file(uint32_t parent_id)
    {
        auto id = next_id();
        auto result = new File(id);
        result->parent_folder_id(parent_id);
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return result;
    }
    RamFileSystem::Symlink *RamFileSystem::new_symlink(uint32_t parent_id, const std::string &link)
    {
        auto id = next_id();
        auto result = new Symlink(id, link);
        result->parent_folder_id(parent_id);
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return result;
    }
    RamFileSystem::CharacterDevice *RamFileSystem::new_char_device(uint32_t parent_id, ICharacterDevice *device)
    {
        auto id = next_id();
        auto result = new CharacterDevice(id, device);
        result->parent_folder_id(parent_id);
        _entry_index[id] = std::unique_ptr<Entry>(result);
        return result;
    }
    // }}}

    // Entry {{{
    RamFileSystem::Entry::Entry(EntryType type, uint32_t id) :
        _id(id),
        _parent_folder_id(NULL_ID),
        _metadata(type)
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
    // }}}
    
    // Symlink {{{
    RamFileSystem::Symlink::Symlink(uint32_t id) :
        Entry(SYMLINK_ENTRY, id)
    {

    }
    RamFileSystem::Symlink::Symlink(uint32_t id, const std::string &link) :
        Entry(SYMLINK_ENTRY, id),
        _link(link)
    {

    }

    void RamFileSystem::Symlink::link(const std::string &link)
    {
        _link = link;
    }
    const std::string &RamFileSystem::Symlink::link() const
    {
        return _link;
    }
    // }}}

    // File {{{
    RamFileSystem::File::File(uint32_t id) :
        Entry(FILE_ENTRY, id)
    {

    }
    uint32_t RamFileSystem::File::size() const
    {
        return static_cast<uint32_t>(_data.size());
    }
    IOResult RamFileSystem::File::read(uint32_t offset, uint32_t count, uint8_t *buffer)
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
    IOResult RamFileSystem::File::write(uint32_t offset, uint32_t count, const uint8_t *data)
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
    // }}}
    
    // Folder {{{
    RamFileSystem::Folder::Folder(uint32_t id) :
        Entry(FOLDER_ENTRY, id)
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
    // }}}

    // MountPoint {{{
    RamFileSystem::MountPoint::MountPoint(uint32_t id, IFileSystem *fs) :
        Entry(MOUNT_POINT_ENTRY, id),
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
    // }}}
    
    // Character Device {{{
    RamFileSystem::CharacterDevice::CharacterDevice(uint32_t id, ICharacterDevice *device) :
        Entry(CHARACTER_DEVICE_ENTRY, id),
        _device(device)
    {

    }

    ICharacterDevice *RamFileSystem::CharacterDevice::device() const
    {
        return _device;
    }
    uint32_t RamFileSystem::CharacterDevice::size() const
    {
        return static_cast<uint32_t>(_device->size());
    }
    IOResult RamFileSystem::CharacterDevice::read(uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        return _device->read(offset, count, buffer);
    }
    IOResult RamFileSystem::CharacterDevice::write(uint32_t offset, uint32_t count, const uint8_t *data)
    {
        return _device->write(offset, count, data);
    }
    // }}}
}
