#include "kernel.h"

#include "fs/ifilesystem.h"
#include "fs/stream_device.h"
#include "fs/ram_filesystem.h"

#include <sstream>

namespace loss
{
    Kernel::Kernel(uint32_t id) :
        _id(id),
        _process_manager(this),
        _dev_fs(nullptr),
        _tty_device(nullptr),
        _root_user(nullptr),
        _keyboard(nullptr)
    {

    }

    ReturnCode Kernel::init()
    {
        if (_vfs.root_filesystem() == nullptr)
        {
            // TODO
            return INTERNAL_ERROR;
        }

        _dev_fs = new loss::RamFileSystem();
        auto result = _vfs.mount("/dev", _dev_fs);
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

        _keyboard = new StreamDevice();
        result = _vfs.create_char_device("/dev/kb0", _keyboard);
        if (result != SUCCESS)
        {
            return result;
        }
        _keyboard->write_string("Hello from keyboard\n");

        kernel_message(true, "Setup VFS");

        result = _user_manager.create_new_user("root", _root_user);
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

        NativeProcess *proc;
        result = _process_manager.create_native_process("/dev/tty0", "kernel", _root_user, proc);
        if (result != SUCCESS)
        {
            std::string error("Error creating kernel process: ");
            error += ReturnCodes::desc(result);
            kernel_message(false, error);
            return result;
        }
        else
        {
            kernel_message(true, "Created kernel process");
            _kernel_proc = proc;
        }

        return SUCCESS;
    }
    ReturnCode Kernel::boot()
    {
        IProcess *proc;
        auto proc_result = _process_manager.create_process_from_file("/etc/init.d", "/dev/tty0", _root_user, proc);
        if (proc_result != SUCCESS)
        {
            return proc_result;
        }

        proc->run(0, nullptr);

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

    User *Kernel::root_user() const
    {
        return _root_user;
    }
    IProcess *Kernel::kernel_proc() const
    {
        return _kernel_proc;
    }

    void Kernel::kernel_message(bool success, const std::string &message)
    {
        std::stringstream ss;
        ss << "[ " << (success ? "SUCCESS" : "FAIL") << " ] " << message << '\n';
        _tty_device->write(0, ss.str().size(), (const uint8_t *)(ss.str().c_str()));
    }
}
