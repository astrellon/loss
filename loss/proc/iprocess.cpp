#include "iprocess.h"

#include "../user.h"

namespace loss
{
    IProcess::IProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel) :
        _info(name, user, id, kernel)
    {

    }

    ProcessInfo &IProcess::info()
    {
        return _info;
    }
}
