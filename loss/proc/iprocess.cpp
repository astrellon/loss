#include "iprocess.h"

#include "../user.h"
#include "../kernel.h"

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
        info().kernel()->virtual_file_system().write_string(info().std_out(), message);
    }
}
