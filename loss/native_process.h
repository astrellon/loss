#pragma once

#include "iprocess.h"

namespace loss
{
    class NativeProcess : public IProcess
    {
        public:
            NativeProcess(const std::string &name, const User *user, uint32_t id);

            virtual int32_t run(int argc, char **argv);
            virtual int32_t shutdown();

    };
}
