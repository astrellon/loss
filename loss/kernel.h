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
    class ProcFileSystem;
    class KernelProcess;
    class KernelStream;
    class KeyboardDriver;

    class Kernel
    {
        public:
            
            Kernel(uint32_t id);

            ReturnCode init();
            ReturnCode boot();
            ReturnCode shutdown();
            ReturnCode run();

            uint32_t id() const;
            bool is_running() const;

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

            User *root_user() const;
            KernelProcess *kernel_proc() const;
            KernelStream *tty_device() const;
            KernelStream *keyboard() const;

        private:

            uint32_t _id;
            bool _running;
            VirtualFileSystem _vfs;
            ProcessManager _process_manager;
            UserManager _user_manager;
            User *_root_user;
            KernelProcess *_kernel_proc;
            ProcFileSystem *_proc_fs;

            RamFileSystem *_dev_fs;
            KernelStream *_tty_device;
            //KeyboardDriver *_keyboard;
            KernelStream *_keyboard;
            void kernel_message(bool success, const std::string &message);
    };
}
