#include "process_info.h"

#include "../fs/ifilesystem_entries.h"

namespace loss
{
    ProcessInfo::ProcessInfo(const std::string &name, const User *user, uint32_t id, Kernel *kernel) :
        _name(name),
        _user(user),
        _id(id),
        _kernel(kernel)
    {

    }

    void ProcessInfo::name(const std::string &name)
    {
        _name = name;
    }
    const std::string &ProcessInfo::name() const
    {
        return _name;
    }

    const User *ProcessInfo::user() const
    {
        return _user;
    }

    uint32_t ProcessInfo::id() const
    {
        return _id;
    }

    Kernel *ProcessInfo::kernel() const
    {
        return _kernel;
    }

    void ProcessInfo::std_out(FileHandle *value)
    {
        _std_out = value;
    }
    FileHandle *ProcessInfo::std_out() const
    {
        return _std_out;
    }
}
