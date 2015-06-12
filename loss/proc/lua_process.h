#pragma once

#include "iprocess.h"
#include "../lua/lua_common.h"

#include <iostream>

namespace loss
{
    class User;
    class Kernel;

    class LuaProcess : public IProcess
    {
        public:
            LuaProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel);

            bool load_string(const std::string &code);

            virtual int32_t run(int argc, char **argv);
            virtual int32_t shutdown();

        private:
            lua_State *_lua;

            void print_stack(std::ostream &output);
            void print_type_value(int n, std::ostream &output, bool includeType = true);
            const char *get_type(int n);
    };
}
