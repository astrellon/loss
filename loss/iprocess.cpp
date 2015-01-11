#include "iprocess.h"

#include "iuser.h"

namespace loss
{
    IProcess::IProcess(const std::string &name, const IUser *user, uint32_t id) :
        _info(name, user, id)
    {

    }

    ProcessInfo &IProcess::info()
    {
        return _info;
    }
}
