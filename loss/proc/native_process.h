#pragma once

#include "iprocess.h"

namespace loss
{
    class Kernel;

    class NativeProcess : public IProcess
    {
        public:
            NativeProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel);

        protected:
            virtual int32_t run_impl();

    };
}
