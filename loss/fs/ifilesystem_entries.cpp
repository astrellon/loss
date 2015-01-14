#include "ifilesystem_entries.h"

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

    IEntry::IEntry(uint32_t parent_id, IFileSystem *fs) :
        _fs(fs),
        _id(0),
        _parent_folder_id(parent_id)
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

    IFileSystem *IEntry::filesystem() const
    {
        return _fs;
    }

    FileEntry::FileEntry(uint32_t parent_id, IFileSystem *fs) :
        IEntry(parent_id, fs),
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

    FolderEntry::FolderEntry() :
        IEntry(0, nullptr)
    {

    }
    FolderEntry::FolderEntry(uint32_t parent_id, IFileSystem *fs) :
        IEntry(parent_id, fs)
    {

    }

    ReturnCode FolderEntry::add_file(const std::string &name, FileEntry *file)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _files[name] = std::unique_ptr<FileEntry>(file);
        return SUCCESS;
    }
    ReturnCode FolderEntry::find_file(const std::string &name, FileEntry **file) const
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

        *file = find->second.get();
        return SUCCESS;
    }

    ReturnCode FolderEntry::add_folder(const std::string &name, FolderEntry *folder)
    {
        bool name_taken = has_entry(name);
        if (name_taken)
        {
            return ENTRY_ALREADY_EXITS;
        }

        _folders[name] = std::unique_ptr<FolderEntry>(folder);
        return SUCCESS;
    }
    ReturnCode FolderEntry::find_folder(const std::string &name, FolderEntry **folder) const
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

        *folder = find->second.get();
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
                entry = find_folder->second.get();
                return SUCCESS;
            }
            return ENTRY_NOT_FOUND;
        }

        entry = find->second.get();
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

    FileHandle::FileHandle(uint32_t id, FileHandle::OpenMode mode, IFileSystem *fs) :
        _id(id),
        _mode(mode),
        _fs(fs)
    {
        if (id == 0u)
        {
            throw std::runtime_error("file entry or process is null");
        }
    }

    uint32_t FileHandle::id() const
    {
        return _id;
    }
    FileHandle::OpenMode FileHandle::mode() const
    {
        return _mode;
    }
    IFileSystem *FileHandle::filesystem() const
    {
        return _fs;
    }
}
