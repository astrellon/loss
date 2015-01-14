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
            IEntry(uint32_t parent_id, IFileSystem *fs);
            
            void id(uint32_t value);
            uint32_t id() const;

            void parent_folder_id(uint32_t id);
            uint32_t parent_folder_id() const;

            MetadataDef &metadata();
            const MetadataDef &metadata() const;

            IFileSystem *filesystem() const;
        private:
            IFileSystem *_fs;
            uint32_t _id;
            uint32_t _parent_folder_id;
            MetadataDef _metadata;
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

            ReturnCode add_file(const std::string &name, FileEntry *file);
            ReturnCode find_file(const std::string &name, FileEntry **file) const;
            ReturnCode add_folder(const std::string &name, FolderEntry *folder);
            ReturnCode find_folder(const std::string &name, FolderEntry **folder) const;
            
            ReturnCode find_entry(const std::string &name, IEntry *entry) const;
            bool has_entry(const std::string &name) const;

            typedef std::map<std::string, std::unique_ptr<FileEntry> > FileMap;
            typedef std::map<std::string, std::unique_ptr<FolderEntry> > FolderMap;

            FileMap::const_iterator begin_files() const;
            FileMap::const_iterator end_files() const;
            uint32_t num_files() const;

            FolderMap::const_iterator begin_folders() const;
            FolderMap::const_iterator end_folders() const;
            uint32_t num_folders() const;

        private:
            uint32_t _id;
            FileMap _files;
            FolderMap _folders;
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

            FileHandle(uint32_t id, OpenMode mode, IFileSystem *fs);

            uint32_t id() const;
            OpenMode mode() const;
            IFileSystem *filesystem() const;

        private:
            uint32_t _id;
            OpenMode _mode;
            IFileSystem *_fs;

    };
            
    inline FileHandle::OpenMode operator |(FileHandle::OpenMode lhs, FileHandle::OpenMode rhs)
    {
        return static_cast<FileHandle::OpenMode>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }
}
