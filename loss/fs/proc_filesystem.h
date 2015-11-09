#pragma once

#include "ram_filesystem.h"

#include <map>
#include <stdint.h>

namespace loss
{
    class Kernel;
    class FileHandle;

    class ProcFileSystem : public RamFileSystem
    {
        public:
            ProcFileSystem(Kernel *kernel);
            
            Kernel *kernel() const;

            void init_base();
            void update_processes();

            virtual ReturnCode read_folder(uint32_t process_id, uint32_t folder_id, FolderEntry &to_populate);

        private:
            Kernel *_kernel;
            uint32_t _current_proc_id;

            FileHandle *create_file_handle(const std::string &filename);

            typedef std::map<uint32_t, uint32_t> ProcessFolderIdMap;
            ProcessFolderIdMap _proc_folder_map;

            uint32_t proc_folder_id(uint32_t process_id);
    };
}
