#pragma once

#include <vector>
#include <map>
#include <stdint.h>
#include <memory>

#include "fs/virtual_fileystem.h"
#include "return_codes.h"
#include "user_manager.h"
#include "proc/process_manager.h"

namespace loss
{
    class RamFileSystem;
    class StreamDevice;

    class Kernel
    {
        public:
            
            Kernel(uint32_t id);

            ReturnCode init();
            ReturnCode boot();
            ReturnCode shutdown();

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

            RamFileSystem *_dev_fs;
            StreamDevice *_tty_device;
            void kernel_message(bool success, const std::string &message);
    };
}
