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
            IProcess();

            virtual int32_t run(int argc, char **argv) = 0;

            ProcessInfo &info();

        private:

            ProcessInfo _info;
    };
}
