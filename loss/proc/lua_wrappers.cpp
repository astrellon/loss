#include "lua_wrappers.h"

#include "lua_process.h"
#include "../kernel.h"
#include "../fs/ifilesystem_entries.h"

#include <string.h>
#include <sstream>

namespace loss
{
    // LuaFile {{{{
    const char *LuaFile::LUA_TABLENAME = "loss_file";
    int LuaFile::LUA_ID = 0x01;

    LuaFile::LuaFile() :
        _file(nullptr)
    {

    }

    void LuaFile::file(FileHandle *file)
    {
        _file = file;
    }
    FileHandle *LuaFile::file() const
    {
        return _file;
    }
    // }}}

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
    
    // New IO Library {{{
    const luaL_Reg LuaWrappers::iolib[] =
    {
        {"close", io_close},
        {"open", io_open},
        {"read", io_read},
        {"write", io_write},
        {NULL, NULL}
    };

    int LuaWrappers::add_io_lib(lua_State *lua)
    {
        luaL_newlib(lua, iolib);   
        add_file_lib(lua);

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
        const char *filename = luaL_checkstring(lua, 1);
        const char *mode = luaL_optstring(lua, 2, "r");
        int i = 0;
        /* check whether 'mode' matches '[rwa]%+?b?' */
        if (!(mode[i] != '\0' && strchr("rwa", mode[i++]) != NULL &&
            (mode[i] != '+' || ++i) &&  /* skip if char is '+' */
            (mode[i] != 'b' || ++i) &&  /* skip if char is 'b' */
            (mode[i] == '\0')))
        {
            return luaL_error(lua, "invalid mode " LUA_QS
                " (should match " LUA_QL("[rwa]%%+?b?") ")", mode);
        }

        auto process = proc(lua);
        auto &info = process->info();
        auto &vfs = info.kernel()->virtual_file_system();

        FileHandle *handle = nullptr;
        vfs.open(info.id(), filename, FileHandle::READ | FileHandle::WRITE, handle);
        if (handle == nullptr)
        {
            return 1;
        }
        
        auto result = new LuaFile();
        result->file(handle);

        wrap_object(lua, result);
        return 1;
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
    
    // New File Library {{{
    const luaL_Reg LuaWrappers::filelib[] =
    {
        {"read", file_read},
        {"write", file_write},
        {"__gc", file_gc},
        {"__tostring", file_tostring},
        {NULL, NULL}
    };
    
    void LuaWrappers::add_file_lib(lua_State *lua)
    {
        luaL_newmetatable(lua, LuaFile::LUA_TABLENAME);
        lua_pushvalue(lua, -1);  /* push metatable */
        lua_setfield(lua, -2, "__index");  /* metatable.__index = metatable */
        luaL_setfuncs(lua, filelib, 0);  /* add file methods to new metatable */
        lua_pop(lua, 1);  /* pop new metatable */
    }

    int LuaWrappers::file_read(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        auto process = proc(lua);
        auto &vfs = process->info().kernel()->virtual_file_system();

        std::stringstream output;
        output << "Reading file: >";
        vfs.read_stream(file->file(), 256, output);
        output << "<\n\n";
        process->write_std_out(output.str());

        return 0;
    }
    int LuaWrappers::file_write(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        return 0;
    }
    int LuaWrappers::file_gc(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        if (file->file() != nullptr)
        {
            auto process = proc(lua);
            process->info().kernel()->virtual_file_system().close(file->file());
        }
        return 0;
    }
    int LuaWrappers::file_tostring(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        if (file->file() == nullptr)
        {
            lua_pushliteral(lua, "file (closed)");
        }
        else
        {
            lua_pushfstring(lua, "file (%p)", file->file());
        }
        return 1;
    }
    // }}}
}
