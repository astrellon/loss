#include "kernel.h"

#include <loss/fs/ifilesystem.h>
#include <loss/fs/stream_device.h>
#include <loss/fs/ram_filesystem.h>

namespace loss
{
    Kernel::Kernel(uint32_t id) :
        _id(id)
    {

    }

    ReturnCode Kernel::init()
    {
        auto ramfs = new loss::RamFileSystem();
        _vfs.root_filesystem(ramfs);

        _vfs.create_folder("/dev");
    
        auto device = new loss::StreamDevice();
        _vfs.create_char_device("/dev/tty0", device);

        loss::FileHandle *handle = nullptr;
        auto result = _vfs.open(1u, "/dev/tty0", loss::FileHandle::WRITE, handle);
        if (result != SUCCESS)
        {
            return result;
        }

        _vfs.write_string(handle, 0, "[ SUCCESS ] - Setup VFS and TTY0\n");

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
}
