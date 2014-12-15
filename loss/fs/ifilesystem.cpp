#include "ifilesystem.h"

namespace loss
{
    MetadataDef::MetadataDef() :
        _owner("No owner")
    {

    }
    const std::string &MetadataDef::owner() const
    {
        return _owner;
    }
    ReturnCode MetadataDef::owner(const std::string &new_owner)
    {
        if (new_owner.empty())
        {
            return NULL_PARAMETER;
        }

        _owner = new_owner;
        return SUCCESS;
    }

    FileEntry::FileEntry() :
        _size(0)
    {

    }
    MetadataDef &FileEntry::metadata()
    {
        return _metadata;
    }
    const MetadataDef &FileEntry::metadata() const
    {
        return _metadata;
    }
    uint32_t FileEntry::size() const
    {
        return _size;
    }
    void FileEntry::size(uint32_t size)
    {
        _size = size;
    }

    MetadataDef &FolderEntry::metadata()
    {
        return _metadata;
    }
    const MetadataDef &FolderEntry::metadata() const
    {
        return _metadata;
    }

    ReturnCode FolderEntry::add_file(const std::string &name, FileEntry *file)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _files[name] = file;
        return SUCCESS;
    }
    ReturnCode FolderEntry::find_file(const std::string &name, FileEntry *file) const
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

    ReturnCode FolderEntry::add_folder(const std::string &name, FolderEntry *folder)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _folders[name] = folder;
        return SUCCESS;
    }
    ReturnCode FolderEntry::find_folder(const std::string &name, FolderEntry *folder) const
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

    ReturnCode FolderEntry::find_entry(const std::string &name, IEntry *entry) const
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
    bool FolderEntry::has_entry(const std::string &name) const
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

    FolderEntry::FileMap::const_iterator FolderEntry::begin_files() const
    {
        return _files.begin();
    }
    FolderEntry::FileMap::const_iterator FolderEntry::end_files() const
    {
        return _files.end();
    }
    uint32_t FolderEntry::num_files() const
    {
        return static_cast<uint32_t>(_files.size());
    }

    FolderEntry::FolderMap::const_iterator FolderEntry::begin_folders() const
    {
        return _folders.begin();
    }
    FolderEntry::FolderMap::const_iterator FolderEntry::end_folders() const
    {
        return _folders.end();
    }
    uint32_t FolderEntry::num_folders() const
    {
        return static_cast<uint32_t>(_folders.size());
    }
}
