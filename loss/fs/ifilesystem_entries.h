#pragma once

#include <string>
#include <stdint.h>
#include <iostream>
#include <memory>

#include <loss/return_codes.h>
#include <loss/fs/common.h>

namespace loss
{
    class IFileSystem;

    class IEntry
    {
        public:
            IEntry(EntryType type, uint32_t parent_id, IFileSystem *fs);
            
            void id(uint32_t value);
            uint32_t id() const;

            void parent_folder_id(uint32_t id);
            uint32_t parent_folder_id() const;

            MetadataDef &metadata();
            const MetadataDef &metadata() const;
            void metadata(const MetadataDef &metadata);

            IFileSystem *filesystem() const;
        private:
            IFileSystem *_fs;
            uint32_t _id;
            uint32_t _parent_folder_id;
            MetadataDef _metadata;
    };

    class SymlinkEntry : public IEntry
    {
        public:
            SymlinkEntry(uint32_t parent_id, IFileSystem *fs, const std::string &link);

            const std::string &link() const;
        private:
            std::string _link;
    };

    class FileEntry : public IEntry
    {
        public:
            FileEntry(uint32_t parent_id, IFileSystem *fs);
            
            uint32_t size() const;
            void size(uint32_t size);

        private:
            uint32_t _size;
    };

    class FolderEntry : public IEntry
    {
        public:
            FolderEntry();
            FolderEntry(uint32_t parent_id, IFileSystem *fs);

            ReturnCode add_entry(const std::string &name, IEntry *entry);
            ReturnCode find_entry(const std::string &name, IEntry **entry) const;
            bool has_entry(const std::string &name) const;

            typedef std::map<std::string, std::unique_ptr<IEntry> > EntryMap;

            EntryMap::const_iterator begin() const;
            EntryMap::const_iterator end() const;
            uint32_t size() const;

        private:
            uint32_t _id;
            EntryMap _entries;
    };

    class FileHandle
    {
        public:
            
            enum OpenMode
            {
                UNKNOWN  = -0x01,
                READ     = 0x01,
                WRITE    = 0x02,
                APPEND   = 0x04
            };

            FileHandle(uint32_t entry_id, uint32_t process_id, OpenMode mode, IFileSystem *fs);

            uint32_t entry_id() const;
            uint32_t process_id() const;
            OpenMode mode() const;
            bool has_write_mode() const;
            bool has_read_mode() const;
            IFileSystem *filesystem() const;

        private:
            uint32_t _entry_id;
            uint32_t _process_id;
            OpenMode _mode;
            IFileSystem *_fs;

    };
            
    inline FileHandle::OpenMode operator |(FileHandle::OpenMode lhs, FileHandle::OpenMode rhs)
    {
        return static_cast<FileHandle::OpenMode>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }
}
