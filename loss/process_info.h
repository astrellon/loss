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
            const std::string &name() const;
            const IUser *user() const;
            uint32_t id() const;
            
            void stdout(std::ostream *value);
            std::ostream *stdout() const;

        private:
            std::string _name;
            const IUser *_user;
            uint32_t _id;

            std::ostream *_stdout;
    };
}

