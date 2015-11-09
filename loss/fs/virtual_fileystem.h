#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>

#include "common.h"
#include "ifilesystem_entries.h"
#include "path.h"
#include "file_handler.h"

#include "../return_codes.h"

namespace loss
{
    class IFileSystem;

    class VirtualFileSystem
    {
        public:
            VirtualFileSystem();

            ReturnCode open(uint32_t process_id, const std::string &name, FileHandle::OpenMode open_mode, FileHandle *&handle);
            ReturnCode close(FileHandle *entry);

            ReturnCode read_folder(uint32_t process_id, const std::string &name, FolderEntry &folder);

            ReturnCode entry_metadata(const std::string &name, MetadataDef &metadata);
            ReturnCode update_entry_metadata(const std::string &name, const MetadataDef &metadata);

            ReturnCode create_file(const std::string &name);
            ReturnCode create_symlink(const std::string &name, const std::string &link);
            ReturnCode create_folder(const std::string &name);
            ReturnCode create_char_device(const std::string &name, ICharacterDevice *device);
            ReturnCode mount(const std::string &name, IFileSystem *fs);
            
            ReturnCode rename(const std::string &name, const std::string &new_name);
            ReturnCode remove_entry(const std::string &name);

            ReturnCode root_filesystem(IFileSystem *fs);
            IFileSystem *root_filesystem() const;
            
            typedef std::vector<std::unique_ptr<IFileSystem> > FileSystems;
            const FileSystems &file_systems() const;
            
            ReturnCode register_file_system(IFileSystem *fs);

            typedef std::vector< FileHandle * > FileHandles;
            ReturnCode find_handles_for_processes(uint32_t process_id, FileHandles &result) const;
            ReturnCode close_process_handles(uint32_t process_id);

        private:
            IFileSystem *_root_filesystem;
            FileSystems _file_systems;
            uint32_t _filesystem_counter;

            FindEntryResult follow_path(const Path &path, uint32_t folder_id);

            // Id is the filesystem_id with the entry_id
            std::map< uint64_t, std::vector< std::unique_ptr<FileHandle> > > _id_to_file_handle;
            std::map< uint32_t, std::vector< uint64_t > > _process_to_id;

            bool find_write_handle(uint32_t entry_id, IFileSystem *fs) const;

            uint64_t make_id(uint32_t entry_id, IFileSystem *fs) const;
    };
}
