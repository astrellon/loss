#pragma once

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

namespace loss
{
    class LuaWrappers
    {
        public:
            static void add_wrappers(lua_State *lua);

        private:
            static void add_base_funcs(lua_State *lua);

            static const luaL_Reg iolib[];
            static int add_io_lib(lua_State *lua);
            static int io_close(lua_State *lua);
            static int io_open(lua_State *lua);
            static int io_read(lua_State *lua);
            static int io_write(lua_State *lua);

            static LuaProcess *proc(lua_State *lua);
    };
}
