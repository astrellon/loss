#include "kernel.h"

#include <loss/fs/ifilesystem.h>
#include <loss/fs/stream_device.h>
#include <loss/fs/ram_filesystem.h>

#include <sstream>

namespace loss
{
    Kernel::Kernel(uint32_t id) :
        _id(id),
        _process_manager(this),
        _init_fs(nullptr),
        _tty_device(nullptr)
    {

    }

    ReturnCode Kernel::init()
    {
        _init_fs = new loss::RamFileSystem();
        auto result = _vfs.root_filesystem(_init_fs);
        if (result != SUCCESS)
        {
            return result;
        }

        result = _vfs.create_folder("/dev");
        if (result != SUCCESS)
        {
            return result;
        }
    
        _tty_device = new StreamDevice();
        result = _vfs.create_char_device("/dev/tty0", _tty_device);
        if (result != SUCCESS)
        {
            return result;
        }

        kernel_message(true, "Setup VFS");

        User *root_user = nullptr;
        result = _user_manager.create_new_user("root", root_user);
        if (result != SUCCESS)
        {
            std::string error("Error creating root user: ");
            error += ReturnCodes::desc(result);
            kernel_message(false, error);
            return result;
        }
        else
        {
            kernel_message(true, "Created root user");
        }

        return SUCCESS;
    }
    ReturnCode Kernel::boot()
    {
        return SUCCESS;
    }
    ReturnCode Kernel::shutdown()
    {
        return SUCCESS;
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

    void Kernel::kernel_message(bool success, const std::string &message)
    {
        std::stringstream ss;
        ss << "[ " << (success ? "SUCCESS" : "FAIL") << " ] " << message << '\n';
        _tty_device->write(0, ss.str().size(), (const uint8_t *)(ss.str().c_str()));
    }
}
