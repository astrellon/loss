#pragma once

#include <vector>
#include <map>
#include <stdint.h>
#include <memory>
#include <vector>

#include <loss/fs/virtual_fileystem.h>
#include <loss/return_codes.h>
#include <loss/iuser.h>
#include <loss/iprocess.h>

namespace loss
{
    class Kernel
    {
        public:
            
            Kernel();

            // Running processes
            // File system
            // Shared memory
            // Timers
            ReturnCode register_user(IUser *user);

            VirtualFileSystem &virtual_file_system();
            const VirtualFileSystem &virtual_file_system() const;

        private:

            typedef std::map<uint32_t, std::unique_ptr<IProcess> > ProcessMap;
            typedef std::map<uint32_t, std::unique_ptr<IUser> > UserMap;
            ProcessMap _processes;
            UserMap  _users;
            VirtualFileSystem _vfs;
    };
}
