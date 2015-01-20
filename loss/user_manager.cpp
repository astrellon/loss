#include "user_manager.h"

namespace loss
{
    UserManager::UserManager() : 
        _id_count(1u)
    {

    }

    ReturnCode UserManager::create_new_user(const std::string &name, User *& result, uint32_t id)
    {
        if (id == 0u)
        {
            id = _id_count;
        }
        else
        {
            const auto existing_user = find_user(id);
            if (existing_user != nullptr)
            {
                return USER_ALREADY_EXISTS;
            }
        }

        auto user = new User(name, id);
        _id_count = id + 1;
        _users[id] = std::unique_ptr<User>(user);
        result = user;
        return SUCCESS;
    }
    ReturnCode UserManager::delete_user(uint32_t id)
    {
        const auto find = _users.find(id);
        if (find == _users.end())
        {
            return CANNOT_FIND_USER;
        }

        // TODO
        _users.erase(find);
        return SUCCESS;
    }
    User *UserManager::find_user(uint32_t id) const
    {
        const auto find = _users.find(id);
        if (find == _users.end())
        {
            return nullptr;
        }

        return find->second.get();
    }

    const UserManager::UserMap &UserManager::users() const
    {
        return _users;
    }
}
