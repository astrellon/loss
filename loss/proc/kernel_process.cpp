#include "kernel_process.h"

#include "../kernel.h"

#include <string>

namespace loss
{
    KernelProcess::KernelProcess(uint32_t id, Kernel *kernel) :
        IProcess("kernel", kernel->root_user(), id, kernel)
    {

    }

    int32_t KernelProcess::run_impl()
    {
        return 0;
    }
}
