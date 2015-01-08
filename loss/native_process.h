#pragma once

#include "iprocess.h"

namespace loss
{
    class NativeProcess : public IProcess
    {
        public:
            NativeProcess(std::ostream *stdout);

            virtual int32_t run(int argc, char **argv);

    };
}
