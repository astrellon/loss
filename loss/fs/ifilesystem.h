#pragma once

#include <string>
#include <stdint.h>
#include <iostream>

#include <loss/return_codes.h>
#include <loss/fs/common.h>

namespace loss
{
    class FolderEntry;
    class FileEntry;
    class ICharacterDeviceEntry;

    class IFileSystem
    {
        public:
            IFileSystem();
            virtual ~IFileSystem();

            virtual IOResult read(uint32_t file_id, uint32_t offset, uint32_t count, uint8_t *buffer) = 0;
            virtual IOResult write(uint32_t file_id, uint32_t offset, uint32_t count, const uint8_t *data) = 0;

            virtual ReturnCode read_folder(uint32_t folder_id, FolderEntry &to_populate) = 0;
            virtual ReturnCode read_symlink(uint32_t symlink_id, std::string &link) = 0;
            virtual FindEntryResult find_entry(uint32_t parent_id, const std::string &name) = 0;
            
            virtual CreateEntryResult create_file(uint32_t folder_id, const std::string &name) = 0;
            virtual CreateEntryResult create_symlink(uint32_t folder_id, const std::string &name, const std::string &link) = 0;
            virtual CreateEntryResult create_folder(uint32_t folder_id, const std::string &name) = 0;
            virtual CreateEntryResult create_char_device(uint32_t folder_id, const std::string &name, ICharacterDeviceEntry *device) = 0;
            virtual CreateEntryResult mount(uint32_t folder_id, const std::string &name, IFileSystem *fs) = 0;

            virtual ReturnCode entry_size(uint32_t entry_id, uint32_t &size) = 0;
            virtual ReturnCode entry_metadata(uint32_t entry_id, MetadataDef &metadata) = 0;
            virtual ReturnCode update_entry_metadata(uint32_t entry_id, const MetadataDef &metadata) = 0;

            virtual ReturnCode remove_entry(uint32_t entry_id) = 0;

            void filesystem_id(uint32_t id);
            uint32_t filesystem_id() const;

            static const uint32_t ROOT_ID;
            static const uint32_t NULL_ID;

        private:
            uint32_t _filesystem_id;
    };
}

