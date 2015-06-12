#include "lua_wrappers.h"

#include "../proc/lua_process.h"

#include "lua_iolib.h"

#include <string.h>
#include <sstream>

namespace loss
{
    void LuaWrappers::add_wrappers(lua_State *lua)
    {
        add_base_funcs(lua);

        // Add new IO lib
        luaL_requiref(lua, "io2", LuaIOLib::add_wrappers, 1);
        lua_pop(lua, 1);
    }

    // Base Functions {{{
    void LuaWrappers::add_base_funcs(lua_State *lua)
    {
        lua_register(lua, "print", [] (lua_State *lua)
        {
            auto process = proc(lua);

            auto n = lua_gettop(lua);  /* number of arguments */
            lua_getglobal(lua, "tostring");

            for (auto i = 1; i <= n; i++)
            {
                lua_pushvalue(lua, -1);  /* function to be called */
                lua_pushvalue(lua, i);   /* value to print */
                lua_call(lua, 1, 1);
                
                auto s = lua_tostring(lua, -1);  /* get result */

                if (s == NULL)
                {
                    return luaL_error(lua,
                        LUA_QL("tostring") " must return a string to " LUA_QL("print"));
                }

                if (i > 1) 
                {
                    process->write_std_out("\t");
                }
                process->write_std_out(s);
                lua_pop(lua, 1);  /* pop result */
            }

            process->write_std_out("\n");
            return 0;
        });
    }
    // }}}
    
}
