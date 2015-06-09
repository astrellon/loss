#pragma once

#include <string>
#include <stdint.h>

#include "process_info.h"

namespace loss
{
    class User;
    class Kernel;

    class IProcess
    {
        public:
            IProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel);

            virtual int32_t run(int argc, char **argv) = 0;
            virtual int32_t shutdown() = 0;

            ProcessInfo &info();

            void write_std_out(const std::string &message);

        private:

            ProcessInfo _info;
    };
}
