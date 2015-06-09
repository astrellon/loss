#pragma once

#include "iprocess.h"

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

#include <iostream>

#define lua_tofloat(L,i)	static_cast<float>(lua_tonumber(L,i))
#define lua_tobool(L,i)		(lua_toboolean(L,i) > 0)
#define lua_isnum(L,i)		(lua_type(L,i) == LUA_TNUMBER)
#define lua_isstr(L,i)		(lua_type(L,i) == LUA_TSTRING)
#define lua_isbool(L,i)		(lua_type(L,i) == LUA_TBOOLEAN)
#define lua_first(L)		lua_pushvalue(L,1); return 1;

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
