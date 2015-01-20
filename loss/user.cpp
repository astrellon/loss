#include "user.h"

namespace loss
{
    User::User(const std::string &name, uint32_t id) :
        _name(name),
        _id(id)
    {

    }

    const std::string &User::name() const
    {
        return _name;
    }
    uint32_t User::id() const
    {
        return _id;
    }
}
