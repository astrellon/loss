#include "lua_wrappers.h"

#include "lua_process.h"

namespace loss
{
    void LuaWrappers::add_wrappers(lua_State *lua)
    {
        add_base_funcs(lua);

        luaL_requiref(lua, "io2", add_io_lib, 1);
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
    
    const luaL_Reg LuaWrappers::iolib[] =
    {
        {"close", io_close},
        {"open", io_open},
        {"read", io_read},
        {"write", io_write},
        {NULL, NULL}
    };

    // New IO Library {{{
    int LuaWrappers::add_io_lib(lua_State *lua)
    {
        luaL_newlib(lua, iolib);   

        return 1;
    }

    int LuaWrappers::io_close(lua_State *lua)
    {
        auto process = proc(lua);
        process->write_std_out("Close!");
        return 0;
    }
    int LuaWrappers::io_open(lua_State *lua)
    {
        return 0;
    }
    int LuaWrappers::io_read(lua_State *lua)
    {
        return 0;
    }
    int LuaWrappers::io_write(lua_State *lua)
    {
        return 0;
    }

    LuaProcess *LuaWrappers::proc(lua_State *lua)
    {
        return reinterpret_cast<LuaProcess *>(lua->process);
    }
    // }}}
}
