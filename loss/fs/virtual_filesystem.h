#pragma once

#include "ifilesystem.h"
#include <map>
#include <vector>
#include <string>

namespace loss
{
    class VirtualFileSystem : public IFileSystem
    {
        public:
            class Entry
            {
                public:
                    MetadataDef &metadata();
                    const MetadataDef &metadata() const;

                private:
                    MetadataDef _metadata;

            };
            class File : public Entry
            {
                public:
                    virtual uint32_t size() const = 0;

                    virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer) = 0;
                    virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data) = 0;
            };
            class DataFile : public File
            {
                public:
                    virtual uint32_t size() const;

                    virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer);
                    virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data);

                private:
                    std::vector<uint8_t> _data;
            };
            class Folder : public Entry
            {
                public:
                    ReturnCode add_file(const std::string &name, File *file);
                    ReturnCode find_file(const std::string &name, File *file) const;
                    ReturnCode add_folder(const std::string &name, Folder *folder);
                    ReturnCode find_folder(const std::string &name, Folder *folder) const;
            
                    ReturnCode find_entry(const std::string &name, Entry *entry) const;
                    bool has_entry(const std::string &name) const;

                    typedef std::map<std::string, File *> FileMap;
                    typedef std::map<std::string, Folder *> FolderMap;

                    FileMap::const_iterator begin_files() const;
                    FileMap::const_iterator end_files() const;
                    uint32_t num_files() const;

                    FolderMap::const_iterator begin_folders() const;
                    FolderMap::const_iterator end_folders() const;
                    uint32_t num_folders() const;

                private:
                    FileMap _files;
                    FolderMap _folders;
            };

            virtual IOResult read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data);

            virtual ReturnCode getdir(const std::string &name, FolderEntry *to_populate);

            ReturnCode add_file(const std::string &name, File *file);
            ReturnCode add_folder(const std::string &name, Folder *folder); 

        private:
            Folder _root;
    };

}
