#pragma once

#include <string>
#include <stdint.h>
#include <iostream>

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
            ReturnCode find_file(const std::string &name, FileEntry *file) const;
            ReturnCode add_folder(const std::string &name, FolderEntry *folder);
            ReturnCode find_folder(const std::string &name, FolderEntry *folder) const;
            
            ReturnCode find_entry(const std::string &name, IEntry *entry) const;
            bool has_entry(const std::string &name) const;

            typedef std::map<std::string, FileEntry *> FileMap;
            typedef std::map<std::string, FolderEntry *> FolderMap;

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
}
