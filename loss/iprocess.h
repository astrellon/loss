#pragma once

#include <string>
#include <stdint.h>
#include "process_info.h"

namespace loss
{
    class IUser;

    class IProcess
    {
        public:
            IProcess(const std::string &name, const IUser *user, uint32_t id);

            virtual int32_t run(int argc, char **argv) = 0;

            ProcessInfo &info();

        private:

            ProcessInfo _info;
    };
}
