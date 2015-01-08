#include "native_process.h"

#include <iostream>

namespace loss
{
    NativeProcess::NativeProcess(std::ostream *stdout) :
        IProcess(stdout)
    {

    }

    int32_t NativeProcess::run(int argc, char **argv)
    {
        *stdout() << "Hello thar\n";
        return 0;
    }
}
