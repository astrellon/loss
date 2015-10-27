#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

#include "../fs/virtual_fileystem.h"

namespace loss
{
    class User;
    class FileHandle;
    class Kernel;

    class ProcessInfo
    {
        public:
            ProcessInfo(const std::string &name, const User *user, uint32_t id, Kernel *kernel);

            void name(const std::string &name);
            const std::string &name() const;
            const User *user() const;
            inline uint32_t id() const
            {
                return _id;
            }
            inline Kernel *kernel() const
            {
                return _kernel;
            }
            inline VirtualFileSystem &vfs() const
            {
                return _vfs;
            }
            
            void std_out(FileHandle *std_out);
            inline FileHandle *std_out() const
            {
                return _std_out;
            }

        private:
            std::string _name;
            const User *_user;
            uint32_t _id;
            Kernel *_kernel;
            VirtualFileSystem &_vfs;

            FileHandle *_std_out;
    };
}

