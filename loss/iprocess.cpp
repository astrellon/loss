#include "iprocess.h"

#include "user.h"

namespace loss
{
    IProcess::IProcess(const std::string &name, const User *user, uint32_t id) :
        _info(name, user, id)
    {

    }

    ProcessInfo &IProcess::info()
    {
        return _info;
    }
}
