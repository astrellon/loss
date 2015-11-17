#pragma once

#include "iprocess.h"

#include <stdint.h>

namespace loss
{
    class Kernel;

    class KernelProcess : public IProcess
    {
        public:
            KernelProcess(uint32_t id, Kernel *kernel);

        protected:
            virtual int32_t run_impl();

    };
}
