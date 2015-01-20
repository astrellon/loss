#pragma once

#include <vector>
#include <map>
#include <stdint.h>
#include <memory>

#include <loss/fs/virtual_fileystem.h>
#include <loss/return_codes.h>
#include <loss/user_manager.h>
#include <loss/process_manager.h>

namespace loss
{
    class Kernel
    {
        public:
            
            Kernel(uint32_t id);

            uint32_t id() const;

            // Running processes
            // File system
            // Shared memory
            // Timers

            VirtualFileSystem &virtual_file_system();
            const VirtualFileSystem &virtual_file_system() const;

            ProcessManager &process_manager();
            const ProcessManager &process_manager() const;

            UserManager &user_manager();
            const UserManager &user_manager() const;

        private:

            uint32_t _id;
            VirtualFileSystem _vfs;
            ProcessManager _process_manager;
            UserManager _user_manager;
    };
}
