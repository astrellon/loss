#pragma once

#include <string>
#include <stdint.h>

namespace loss
{
    class User
    {
        public:
            User(const std::string &name, uint32_t id);

            const std::string &name() const;
            uint32_t id() const;

        private:
            std::string _name;
            uint32_t _id;
    };
}
