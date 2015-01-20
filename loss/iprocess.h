#pragma once

#include <string>
#include <stdint.h>
#include "process_info.h"

namespace loss
{
    class User;

    class IProcess
    {
        public:
            IProcess(const std::string &name, const User *user, uint32_t id);

            virtual int32_t run(int argc, char **argv) = 0;
            virtual int32_t shutdown() = 0;

            ProcessInfo &info();

        private:

            ProcessInfo _info;
    };
}
