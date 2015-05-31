#include "kernel.h"

#include <loss/fs/ifilesystem.h>
#include <loss/fs/stream_device.h>
#include <loss/fs/ram_filesystem.h>

#include <sstream>

namespace loss
{
    Kernel::Kernel(uint32_t id) :
        _id(id),
        _init_fs(nullptr),
        _tty_device(nullptr)
    {

    }

    ReturnCode Kernel::init()
    {
        _init_fs = new loss::RamFileSystem();
        _vfs.root_filesystem(_init_fs);

        _vfs.create_folder("/dev");
    
        _tty_device = new StreamDevice();
        _vfs.create_char_device("/dev/tty0", _tty_device);

        kernel_message(true, "Setup VFS");

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

    ReturnCode Kernel::run_program(const std::string &path)
    {
        FileHandle *handle = nullptr;
        auto result = _vfs.open(1u, "/init.d", FileHandle::READ, handle);
        if (result != SUCCESS)
        {
            return result;
        }

        return SUCCESS;
    }

    void Kernel::kernel_message(bool success, const std::string &message)
    {
        std::stringstream ss;
        ss << "[ " << (success ? "SUCCESS" : "FAIL") << " ] " << message << '\n';
        _tty_device->write(0, ss.str().size(), (const uint8_t *)(ss.str().c_str()));
    }
}
