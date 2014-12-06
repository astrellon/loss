#pragma once

#include <string>
#include <stdint.h>

namespace loss
{
    class IUser
    {
        public:
            virtual const std::string &name() const = 0;
            virtual const uint32_t id() const = 0;
    };
}
