#pragma once

#include <vector>
#include <map>
#include <stdint.h>

namespace loss
{
    class IUser;
    class IProcess;
    class IFileSystem;

    class System
    {
        public:
            System();

            // Running processes
            // File system
            // Shared memory
            // Timers
            
        private:
            std::map<uint32_t, IProcess *> _processes;
            std::map<uint32_t, IUser *> _users;
            IFileSystem *_filesystem;
    };
}
