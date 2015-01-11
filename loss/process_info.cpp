#include "process_info.h"

namespace loss
{
    ProcessInfo::ProcessInfo(const std::string &name, const IUser *user, uint32_t id) :
        _name(name),
        _user(user),
        _id(id)
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

    const IUser *ProcessInfo::user() const
    {
        return _user;
    }

    uint32_t ProcessInfo::id() const
    {
        return _id;
    }

    void ProcessInfo::std_out(std::ostream *value)
    {
        _std_out = value;
    }
    std::ostream *ProcessInfo::std_out() const
    {
        return _std_out;
    }


}
