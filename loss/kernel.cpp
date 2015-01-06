#include "kernel.h"

#include <loss/iuser.h>
#include <loss/fs/ifilesystem.h>

namespace loss
{
    Kernel::Kernel()
    {

    }

    ReturnCode Kernel::register_user(IUser *user)
    {
        if (user == nullptr)
        {
            return NULL_PARAMETER;
        }

        auto find = _users.find(user->id());
        if (find != _users.end())
        {
            return ALREADY_IN_LIST;
        }

        _users[user->id()] = std::unique_ptr<IUser>(user);
        return SUCCESS;
    }
    VirtualFileSystem &Kernel::virtual_file_system()
    {
        return _vfs;
    }
    const VirtualFileSystem &Kernel::virtual_file_system() const
    {
        return _vfs;
    }
}
