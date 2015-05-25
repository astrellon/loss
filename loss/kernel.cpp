#include "kernel.h"

#include <loss/fs/ifilesystem.h>

namespace loss
{
    Kernel::Kernel(uint32_t id) :
        _id(id)
    {

    }

    uint32_t Kernel::id() const
    {
        return _id;
    }

    VirtualFileSystem &Kernel::virtual_file_system()
    {
        return _vfs;
    }
    const VirtualFileSystem &Kernel::virtual_file_system() const
    {
        return _vfs;
    }

    ProcessManager &Kernel::process_manager()
    {
        return _process_manager;
    }
    const ProcessManager &Kernel::process_manager() const
    {
        return _process_manager;
    }

    UserManager &Kernel::user_manager()
    {
        return _user_manager;
    }
    const UserManager &Kernel::user_manager() const
    {
        return _user_manager;
    }

    TTYManager &Kernel::tty_manager()
    {
        return _tty_manager;
    }
    const TTYManager &Kernel::tty_manager() const
    {
        return _tty_manager;
    }
}
