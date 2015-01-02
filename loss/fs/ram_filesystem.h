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
                    virtual ~Entry();

                    uint32_t id() const;

                    void parent_folder_id(uint32_t id);
                    uint32_t parent_folder_id() const;

                    MetadataDef &metadata();
                    const MetadataDef &metadata() const;

                    void metadata(const MetadataDef &metadata);

                private:
                    uint32_t _id;
                    uint32_t _parent_folder_id;
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

                    ReturnCode add_entry(const std::string &name, Entry *entry);
                    ReturnCode remove_entry(const std::string &name);
                    ReturnCode remove_entry(uint32_t id);
                    ReturnCode find_entry(const std::string &name, Entry **entry) const;
                    bool has_entry(const std::string &name) const;

                    typedef std::map<std::string, Entry *> EntryMap;

                    EntryMap::const_iterator begin() const;
                    EntryMap::const_iterator end() const;
                    uint32_t size() const;

                private:
                    EntryMap _entries;
            };
            // }}}
            
            // MountPoint Entry {{{
            class MountPoint : public Entry
            {
                public:
                    MountPoint(uint32_t id, IFileSystem *fs);

                    IFileSystem *fs() const;
                private:
                    IFileSystem *_fs;
            };
            // }}}

            virtual IOResult read(uint32_t file_id, uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(uint32_t file_id, uint32_t offset, uint32_t count, const uint8_t *data);

            virtual ReturnCode read_folder(uint32_t folder_id, FolderEntry *to_populate);
            virtual FindEntryResult find_entry(uint32_t parent_id, const std::string &name);
            
            virtual CreateEntryResult create_file(uint32_t parent_id, const std::string &name);
            virtual CreateEntryResult create_folder(uint32_t parent_id, const std::string &name);
            virtual CreateEntryResult mount(uint32_t parent_id, const std::string &name, IFileSystem *fs);

            virtual ReturnCode entry_size(uint32_t entry_id, uint32_t &size);
            virtual ReturnCode entry_metadata(uint32_t entry_id, MetadataDef &metadata);
            virtual ReturnCode update_entry_metadata(uint32_t entry_id, const MetadataDef &metadata);

            virtual ReturnCode remove_entry(uint32_t entry_id);

            ReturnCode add_file(const std::string &name, File *file);
            ReturnCode add_folder(const std::string &name, Folder *folder); 

        private:
            Folder _root;
            uint32_t _id_counter;
            std::map<uint32_t, Entry *> _entry_index;

            uint32_t next_id();

            Folder *new_folder(uint32_t parent_id);
            DataFile *new_file(uint32_t parent_id);

            CreateEntryResult add_entry(uint32_t folder_id, const std::string &name, Entry *entry);
            ReturnCode remove_entry(uint32_t parent_id, uint32_t entry_id);
    };
}

