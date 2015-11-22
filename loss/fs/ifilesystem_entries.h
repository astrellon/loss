#pragma once

#include <string>
#include <stdint.h>
#include <iostream>
#include <memory>

#include "../return_codes.h"
#include "../stream_handler.h"
#include "../fs/common.h"

namespace loss
{
    class IFileSystem;
    class IProcess;

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

    class ICharacterDevice
    {
        public:
            virtual uint32_t size() const = 0;
            virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer) = 0;
            virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data) = 0;

            virtual IOResult read_string(std::stringstream &buffer);
            virtual IOResult write_string(const std::string &data);
    };

    class CharacterDeviceEntry : public IEntry
    {
        public:
            CharacterDeviceEntry(uint32_t parent_id, IFileSystem *fs, ICharacterDevice *device);

            ICharacterDevice *device() const;

            uint32_t size() const;
        private:
            ICharacterDevice *_device;

    };
}
