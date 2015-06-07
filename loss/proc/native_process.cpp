#include "native_process.h"

#include <iostream>

#include "../fs/ifilesystem_entries.h"
#include "../kernel.h"

namespace loss
{
    NativeProcess::NativeProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel) :
        IProcess(name, user, id, kernel)
    {

    }

    int32_t NativeProcess::run(int argc, char **argv)
    {
        //*info().std_out() << "Hello thar\n";
        //info().std_out()->write
        info().kernel()->virtual_file_system().write_string(info().std_out(), "Hello thar\n");
        return 0;
    }
    int32_t NativeProcess::shutdown()
    {
        return 0;
    }
}
