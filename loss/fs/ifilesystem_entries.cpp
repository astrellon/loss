#include "ifilesystem_entries.h"

#include "ifilesystem.h"

#include <sstream>

namespace loss
{
    // IEntry {{{
    IEntry::IEntry(EntryType type, uint32_t parent_id, IFileSystem *fs) :
        _fs(fs),
        _id(0),
        _parent_folder_id(parent_id),
        _metadata(type)
    {

    }

    void IEntry::id(uint32_t value)
    {
        _id = value;
    }
    uint32_t IEntry::id() const
    {
        return _id;
    }
    
    void IEntry::parent_folder_id(uint32_t id)
    {
        _parent_folder_id = id;
    }
    uint32_t IEntry::parent_folder_id() const
    {
        return _parent_folder_id;
    }

    MetadataDef &IEntry::metadata()
    {
        return _metadata;
    }
    const MetadataDef &IEntry::metadata() const
    {
        return _metadata;
    }
    void IEntry::metadata(const MetadataDef &metadata)
    {
        _metadata = metadata;
    }

    IFileSystem *IEntry::filesystem() const
    {
        return _fs;
    }
    // }}}
    
    // SymlinkEntry {{{
    SymlinkEntry::SymlinkEntry(uint32_t parent_id, IFileSystem *fs, const std::string &link) :
        IEntry(SYMLINK_ENTRY, parent_id, fs),
        _link(link)
    {

    }

    const std::string &SymlinkEntry::link() const
    {
        return _link;
    }
    // }}}

    // FileEntry {{{
    FileEntry::FileEntry(uint32_t parent_id, IFileSystem *fs) :
        IEntry(FILE_ENTRY, parent_id, fs),
        _size(0)
    {

    }

    uint32_t FileEntry::size() const
    {
        return _size;
    }
    void FileEntry::size(uint32_t size)
    {
        _size = size;
    }
    // }}}

    // FolderEntry {{{
    FolderEntry::FolderEntry() :
        IEntry(FOLDER_ENTRY, 0, nullptr)
    {

    }
    FolderEntry::FolderEntry(uint32_t parent_id, IFileSystem *fs) :
        IEntry(FOLDER_ENTRY, parent_id, fs)
    {

    }

    ReturnCode FolderEntry::add_entry(const std::string &name, IEntry *entry)
    {
        if (name.empty() || entry == nullptr)
        {
            return NULL_PARAMETER;
        }

        auto name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _entries[name] = std::unique_ptr<IEntry>(entry);

        return SUCCESS;
    }

    ReturnCode FolderEntry::find_entry(const std::string &name, IEntry **entry) const
    {
        if (name.empty() || entry == nullptr)
        {
            return NULL_PARAMETER;
        }
        auto find = _entries.find(name);
        if (find == _entries.end())
        {
            return ENTRY_NOT_FOUND;
        }

        *entry = find->second.get();
        return SUCCESS;
    }
    bool FolderEntry::has_entry(const std::string &name) const
    {
        auto find = _entries.find(name);
        if (find != _entries.end())
        {
            return true;
        }
        return false;
    }

    FolderEntry::EntryMap::const_iterator FolderEntry::begin() const
    {
        return _entries.begin();
    }
    FolderEntry::EntryMap::const_iterator FolderEntry::end() const
    {
        return _entries.end();
    }
    uint32_t FolderEntry::size() const
    {
        return static_cast<uint32_t>(_entries.size());
    }
    // }}}
    
    // ICharacterDevice {{{
    ICharacterDevice::ICharacterDevice(Kernel *kernel) :
        _kernel(kernel)
    {

    }

    IOResult ICharacterDevice::read_string(std::stringstream &buffer)
    {
        auto read_total = 0u;
        uint8_t temp[128];
        do
        {
            auto result = read(0, 127, temp);
            if (result.status() != SUCCESS)
            {
                return result;
            }

            read_total += result.bytes();
            temp[result.bytes()] = '\0';
            buffer.write(reinterpret_cast<const char *>(temp), result.bytes());
        } while (size() > 0u);

        return IOResult(read_total, SUCCESS);
    }
    IOResult ICharacterDevice::write_string(const std::string &data)
    {
        return write(0, data.size(), reinterpret_cast<const uint8_t *>(data.c_str()));
    }
    // }}}
    
    // CharacterDeviceEntry {{{
    CharacterDeviceEntry::CharacterDeviceEntry(uint32_t parent_id, IFileSystem *fs, ICharacterDevice *device) :
        IEntry(CHARACTER_DEVICE_ENTRY, parent_id, fs),
        _device(device)
    {

    }

    uint32_t CharacterDeviceEntry::size() const
    {
        return _device->size();
    }

    ICharacterDevice *CharacterDeviceEntry::device() const
    {
        return _device;
    }
    // }}}
}
