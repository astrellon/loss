#pragma once

#include "user.h"
#include <map>
#include <stdint.h>
#include <memory>

#include <loss/return_codes.h>

namespace loss
{
    class UserManager
    {
        public:
            UserManager();

            ReturnCode create_new_user(const std::string &name, User *& result, uint32_t id = 0u);
            ReturnCode delete_user(uint32_t id);
            User *find_user(uint32_t id) const;
            
            typedef std::map<uint32_t, std::unique_ptr<User> > UserMap;
            const UserMap &users() const;

        private:
            UserMap _users;
            uint32_t _id_count;

    };
}
