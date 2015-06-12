#pragma once

#include "lua_common.h"

namespace loss
{
    class LuaProcess;
    class FileHandle;

    class LuaWrappers
    {
        public:
            static void add_wrappers(lua_State *lua);

        private:
            static void add_base_funcs(lua_State *lua);

    };
}
