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

    class IFileSystem
    {
        public:
            virtual ~IFileSystem();

            virtual IOResult read(uint32_t folder_id, const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer) = 0;
            virtual IOResult write(uint32_t folder_id, const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data) = 0;

            virtual ReturnCode create_file(uint32_t folder_id, const std::string &name) = 0;
            virtual ReturnCode create_folder(uint32_t folder_id, const std::string &name) = 0;
            virtual ReturnCode read_folder(uint32_t folder_id, const std::string &name, FolderEntry *to_populate) = 0;
            virtual FindFolderResult find_folder(uint32_t folder_id, const std::string &name) = 0;
    };
}

