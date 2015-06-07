#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

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
            uint32_t id() const;
            Kernel *kernel() const;
            
            void std_out(FileHandle *std_out);
            FileHandle *std_out() const;

        private:
            std::string _name;
            const User *_user;
            uint32_t _id;
            Kernel *_kernel;

            FileHandle *_std_out;
    };
}

