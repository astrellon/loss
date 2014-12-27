#pragma once

#include "ifilesystem.h"
#include "ifilesystem_enties.h"
#include <map>
#include <vector>
#include <string>

namespace loss
{
    class FolderEntry;

    class RamFileSystem : public IFileSystem
    {
        public:
            RamFileSystem();

            // BaseEntry {{{
            class Entry
            {
                public:
                    Entry(uint32_t id);

                    uint32_t id() const;

                    MetadataDef &metadata();
                    const MetadataDef &metadata() const;

                private:
                    uint32_t _id;
                    MetadataDef _metadata;

            };
            // }}}
            
            // File Entry {{{
            class File : public Entry
            {
                public:
                    File(uint32_t id);

                    virtual uint32_t size() const = 0;

                    virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer) = 0;
                    virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data) = 0;
            };
            class DataFile : public File
            {
                public:
                    DataFile(uint32_t id);

                    virtual uint32_t size() const;

                    virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer);
                    virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data);

                private:
                    std::vector<uint8_t> _data;
            };
            // }}}
            
            // Folder Entry {{{
            class Folder : public Entry
            {
                public:
                    Folder(uint32_t id);

                    ReturnCode add_file(const std::string &name, File *file);
                    ReturnCode find_file(const std::string &name, File **file) const;
                    ReturnCode add_folder(const std::string &name, Folder *folder);
                    ReturnCode find_folder(const std::string &name, Folder **folder) const;
            
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
            // }}}

            virtual IOResult read(FolderEntry *folder, const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(FolderEntry *folder, const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data);
            virtual ReturnCode create_folder(FolderEntry *folder, const std::string &name);

            virtual ReturnCode read_folder(FolderEntry *folder, const std::string &name, FolderEntry *to_populate);
            virtual FindFolderResult find_folder(uint32_t id, const std::string &name);

            ReturnCode add_file(const std::string &name, File *file);
            ReturnCode add_folder(const std::string &name, Folder *folder); 

        private:
            Folder _root;
            uint32_t _id_counter;
            std::map<uint32_t, File *> _file_index;
            std::map<uint32_t, Folder *> _folder_index;

            uint32_t next_id();

            Folder *new_folder();
            DataFile *new_file();
    };
}

