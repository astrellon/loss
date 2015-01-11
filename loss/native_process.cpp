#include "native_process.h"

#include <iostream>

namespace loss
{
    NativeProcess::NativeProcess(const std::string &name, const IUser *user, uint32_t id) :
        IProcess(name, user, id)
    {

    }

    int32_t NativeProcess::run(int argc, char **argv)
    {
        *info().std_out() << "Hello thar\n";
        return 0;
    }
}
