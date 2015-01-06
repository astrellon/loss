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
    class IEntry;
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
            IOResult read(FileEntry *entry, uint32_t offset, uint32_t count, uint8_t *buffer);
            ReturnCode read_folder(const std::string &name, FolderEntry *folder);

            IOResult write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data);
            IOResult write(FileEntry *entry, uint32_t offset, uint32_t count, const uint8_t *data);
            
            ReturnCode entry_size(const std::string &name, uint32_t &size);
            ReturnCode entry_size(IEntry *entry, uint32_t &size);

            ReturnCode entry_metadata(const std::string &name, MetadataDef &metadata);
            ReturnCode entry_metadata(IEntry *entry, MetadataDef &metadata);
            ReturnCode update_entry_metadata(const std::string &name, const MetadataDef &metadata);
            ReturnCode update_entry_metadata(IEntry *entry, const MetadataDef &metadata);

            ReturnCode create_file(const std::string &name);
            ReturnCode create_folder(const std::string &name);
            ReturnCode mount(const std::string &name, IFileSystem *fs);
            
            ReturnCode remove_entry(const std::string &name);
            ReturnCode remove_entry(IEntry *entry);

            // Helpers
            IOResult read_stream(const std::string &name, uint32_t offset, uint32_t count, std::ostream &ss);
            IOResult write_string(const std::string &name, uint32_t offset, const std::string &data);

            ReturnCode root_filesystem(IFileSystem *fs);
            IFileSystem *root_filesystem() const;
            
            typedef std::vector<std::unique_ptr<IFileSystem> > FileSystems;
            const FileSystems &file_systems() const;
            
            ReturnCode register_file_system(IFileSystem *fs);

        private:
            IFileSystem *_root_filesystem;
            FileSystems _file_systems;

            FindEntryResult follow_path(const Path &path, uint32_t folder_id);
    };
}
