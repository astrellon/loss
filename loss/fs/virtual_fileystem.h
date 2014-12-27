#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>

#include <loss/return_codes.h>
#include <loss/fs/common.h>

#include "path.h"

namespace loss
{
    class IFileSystem;
    class FileEntry;
    class FolderEntry;

    class VirtualFileSystem
    {
        public:
            VirtualFileSystem();

            //virtual ReturnCode open(const std::string &name) = 0;
            //virtual ReturnCode symlink(const std::string &link_filename, const std::string &destination) = 0;
            
            // Change to a stream version at some point.
            IOResult read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer);
            IOResult write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data);
            ReturnCode create_folder(const std::string &name);

            // Helpers
            IOResult read_stream(const std::string &name, uint32_t offset, uint32_t count, std::ostream &ss);
            IOResult write_string(const std::string &name, uint32_t offset, const std::string &data);

            void root_filesystem(IFileSystem *fs);
            IFileSystem *root_filesystem() const;

        private:
            IFileSystem *_root_filesystem;

            FindFolderResult follow_path(const Path &path, uint32_t folder_id = 1);
    };
}
