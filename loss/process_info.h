#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

namespace loss
{
    class IUser;

    class ProcessInfo
    {
        public:
            ProcessInfo(const std::string &name, const IUser *user, uint32_t id);

            void name(const std::string &name);
            const std::string &name() const;
            const IUser *user() const;
            uint32_t id() const;
            
            void std_out(std::ostream *value);
            std::ostream *std_out() const;

        private:
            std::string _name;
            const IUser *_user;
            uint32_t _id;

            std::ostream *_std_out;
    };
}

