#include "iprocess.h"

#include "../user.h"
#include "../kernel.h"
#include "../fs/virtual_fileystem.h"

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
            
    void IProcess::write_std_out(const std::string &message)
    {
        if (info().std_out() != nullptr)
        {
            info().vfs().write_string(info().std_out(), message);
        }
    }
}
