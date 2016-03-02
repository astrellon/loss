#include "lua_iolib.h"

#include "../fs/ifilesystem_entries.h"
#include "../proc/lua_process.h"
#include "../kernel.h"

#include <sstream>
#include <string.h>

namespace loss
{
    // LuaFile {{{
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
    
    // New IO Library {{{
    const luaL_Reg LuaIOLib::iolib[] =
    {
        {"rename", io_rename},
        {"delete", io_delete},

        {"open", io_open},
        {"read_dir", io_read_dir},
        {"make_dir", io_make_dir},
        {NULL, NULL}
    };

    int LuaIOLib::add_wrappers(lua_State *lua)
    {
        luaL_newlib(lua, iolib);   
        add_file_lib(lua);

        return 1;
    }

    int LuaIOLib::io_rename(lua_State *lua)
    {
        auto process = proc(lua);
        auto from = luaL_checkstring(lua, 1);
        auto to = luaL_checkstring(lua, 2);

        auto result = process->vfs().rename(std::string(from), std::string(to));
        lua_pushinteger(lua, result);

        return 1;
    }
    int LuaIOLib::io_delete(lua_State *lua)
    {
        auto process = proc(lua);
        auto filename = luaL_checkstring(lua, 1);

        auto result = process->vfs().remove_entry(filename);
        lua_pushinteger(lua, result);

        return 1;
    }
    int LuaIOLib::io_open(lua_State *lua)
    {
        const char *filename = luaL_checkstring(lua, 1);
        const char *mode = luaL_optstring(lua, 2, "r");
        int i = 0;
        
        /* check whether 'mode' matches '[rwa]%+?b?' */
        auto file_mode = FileHandle::NONE;
        while (mode[i] != '\0')
        {
            auto m = mode[i++];
            if (m == 'r')
            {
                file_mode |= FileHandle::READ;
            }
            else if (m == 'w')
            {
                file_mode |= FileHandle::WRITE;
            }
            else if (m == 'a')
            {
                file_mode |= FileHandle::APPEND;
            }
            else if (m == '+')
            {
                file_mode |= FileHandle::READ | FileHandle::WRITE;
            }
            else
            {
                return luaL_error(lua, "invalid mode " LUA_QS
                    " (should match " LUA_QL("[rwa]%%+?b?") ")", mode);
            }
        }

        auto process = proc(lua);
        auto &info = process->info();
        auto &vfs = info.vfs();

        FileHandle *handle = nullptr;
        vfs.open(info.id(), filename, file_mode, handle);
        if (handle == nullptr)
        {
            if ((file_mode & (FileHandle::WRITE | FileHandle::APPEND)) > 0)
            {
                if (vfs.create_file(filename) != ReturnCode::SUCCESS)
                {
                    return luaL_fileresult(lua, 0, filename);
                }

                vfs.open(info.id(), filename, file_mode, handle);
                if (handle == nullptr)
                {
                    return luaL_fileresult(lua, 0, filename);
                }
            }
            else
            {
                return luaL_fileresult(lua, 0, filename);
            }
        }
        
        auto result = new LuaFile();
        result->file(handle);

        wrap_object(lua, result);
        return 1;
    }
    int LuaIOLib::io_read_dir(lua_State *lua)
    {
        return 0;
    }
    int LuaIOLib::io_make_dir(lua_State *lua)
    {
        auto process = proc(lua);
        auto filename = luaL_checkstring(lua, 1);

        auto result = process->vfs().create_folder(filename);
        lua_pushinteger(lua, result);

        return 1;
    }
    // }}}
    
    // New File Library {{{
    const luaL_Reg LuaIOLib::filelib[] =
    {
        {"read", file_read},
        {"write", file_write},
        {"close", file_close},
        {"lines", file_lines},
        {"__gc", file_gc},
        {"__tostring", file_tostring},
        {NULL, NULL}
    };
    
    void LuaIOLib::add_file_lib(lua_State *lua)
    {
        luaL_newmetatable(lua, LuaFile::LUA_TABLENAME);
        lua_pushvalue(lua, -1);  /* push metatable */
        lua_setfield(lua, -2, "__index");  /* metatable.__index = metatable */
        luaL_setfuncs(lua, filelib, 0);  /* add file methods to new metatable */
        lua_pop(lua, 1);  /* pop new metatable */
    }

    static int io_readline(lua_State *lua)
    {
        return 0;
    }
    int LuaIOLib::file_read(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        return g_read(lua, file->file(), 2);
    }
    int LuaIOLib::file_write(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        lua_pushvalue(lua, 1);
        return g_write(lua, file->file(), 2);
    }
    int LuaIOLib::file_close(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        file->file()->close();
        return 0;
    }
    int LuaIOLib::file_lines (lua_State *lua) {
        int i;
        int n = lua_gettop(lua) - 1;  /* number of arguments to read */
        /* ensure that arguments will fit here and into 'io_readline' stack */
        luaL_argcheck(lua, n <= LUA_MINSTACK - 3, LUA_MINSTACK - 3, "too many options");
        lua_pushvalue(lua, 1);  /* file handle */
        lua_pushinteger(lua, n);  /* number of arguments to read */
        lua_pushboolean(lua, false);  /* close/not close file when finished */

        for (i = 1; i <= n; i++) 
        {
            lua_pushvalue(lua, i + 1);  /* copy arguments */
        }

        lua_pushcclosure(lua, io_readline, 3 + n);
        return 1;
    }

    int LuaIOLib::io_readline (lua_State *lua) {
        auto file = cast_upvalue_userdata<LuaFile>(lua, 1);
        int i;
        int n = (int)lua_tointeger(lua, lua_upvalueindex(2));
        /*
        if (file->fil
            return luaL_error(lua, "file is already closed");
        */
        lua_settop(lua , 1);
        for (i = 1; i <= n; i++)  /* push arguments to 'g_read' */
        {
            lua_pushvalue(lua, lua_upvalueindex(3 + i));
        }
        n = g_read(lua, file->file(), 2);  /* 'n' is number of results */
        lua_assert(n > 0);  /* should return at least a nil */
        if (!lua_isnil(lua, -n))  /* read at least one value? */
        {
            return n;  /* return them */
        }
        else 
        {  /* first result is nil: EOF or error */
            if (n > 1) 
            {  /* is there error information? */
                /* 2nd result is error message */
                return luaL_error(lua, "%s", lua_tostring(lua, -n + 1));
            }
            if (lua_toboolean(lua, lua_upvalueindex(3))) 
            {  /* generator created file? */
                lua_settop(lua, 0);
                lua_pushvalue(lua, lua_upvalueindex(1));
                file->file()->close();
            }
            return 0;
        }
    }
    int LuaIOLib::file_gc(lua_State *lua)
    {
        auto file = cast_userdata<LuaFile>(lua, 1);
        if (file->file() != nullptr)
        {
            auto process = proc(lua);
            process->info().kernel()->virtual_file_system().close(file->file());
        }
        return 0;
    }
    int LuaIOLib::file_tostring(lua_State *lua)
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

    // File Read Operatings {{{
    int LuaIOLib::g_read (lua_State *lua, FileHandle *file, int first) 
    {
        auto process = proc(lua);
        auto &vfs = process->vfs();

        int nargs = lua_gettop(lua) - 1;
        int success;
        int n;
        //clearerr(file);
        if (nargs == 0) {  /* no arguments? */
            success = read_line(lua, file, 1);
            n = first+1;  /* to return 1 result */
        }
        else {  /* ensure stack space for all results and for auxlib's buffer */
            luaL_checkstack(lua, nargs+LUA_MINSTACK, "too many arguments");
            success = 1;
            for (n = first; nargs-- && success; n++) {
                if (lua_type(lua, n) == LUA_TNUMBER) {
                    size_t l = (size_t)lua_tointeger(lua, n);
                    success = (l == 0) ? file->at_end_of_stream() : read_chars(lua, file, l);
                }
                else {
                    const char *p = lua_tostring(lua, n);
                    luaL_argcheck(lua, p && p[0] == '*', n, "invalid option");
                    switch (p[1]) {
                        case 'n':  /* number */
                            success = read_number(lua, file);
                            break;
                        case 'l':  /* line */
                            success = read_line(lua, file, 1);
                            break;
                        case 'L':  /* line with end-of-line */
                            success = read_line(lua, file, 0);
                            break;
                        case 'a':  /* file */
                            read_all(lua, file);  /* read entire file */
                            success = 1; /* always success */
                            break;
                        default:
                            return luaL_argerror(lua, n, "invalid format");
                    }
                }
            }
        }
        /*
        if (ferror(file))
            return luaL_fileresult(lua, 0, NULL);
            */
        if (!success) {
            lua_pop(lua, 1);  /* remove last result */
            lua_pushnil(lua);  /* push nil instead */
        }
        return n - first;
    }

    int LuaIOLib::read_line (lua_State *lua, FileHandle *file, int chop) 
    {
        auto process = proc(lua);
        auto &vfs = process->vfs();

        luaL_Buffer b;
        luaL_buffinit(lua, &b);
        for (;;) 
        {
            size_t l;
            char *p = luaL_prepbuffer(&b);
            auto read_result = file->read_till_character('\n', LUAL_BUFFERSIZE, (uint8_t*)p);
            if (read_result.bytes() == 0)
            {
                luaL_pushresult(&b);  /* close buffer */
                return (lua_rawlen(lua, -1) > 0);  /* check whether read something */
            }
            l = strlen(p);

            if (l == 0 || p[l-1] != '\n')
            {
                luaL_addsize(&b, l);
            }
            else 
            {
                luaL_addsize(&b, l - chop);  /* chop 'eol' if needed */
                luaL_pushresult(&b);  /* close buffer */
                return 1;  /* read at least an `eol' */
            }
        }
    }

    int LuaIOLib::read_chars (lua_State *lua, FileHandle *file, size_t n) 
    {
        auto process = proc(lua);
        auto &vfs = process->vfs();
        
        //size_t nr;  /* number of chars actually read */
        char *p;
        luaL_Buffer b;
        luaL_buffinit(lua, &b);
        p = luaL_prepbuffsize(&b, n);  /* prepare buffer to read whole block */
        auto read_result = file->read(n, (uint8_t*)p);
        //nr = fread(p, sizeof(char), n, file);  /* try to read 'n' chars */
        luaL_addsize(&b, read_result.bytes());
        luaL_pushresult(&b);  /* close buffer */
        return (read_result.bytes() > 0);  /* true iff read something */
    }

#define MAX_SIZE_T	(~(size_t)0)
    void LuaIOLib::read_all (lua_State *lua, FileHandle *file) 
    {
        auto process = proc(lua);
        auto &vfs = process->vfs();
        
        size_t rlen = LUAL_BUFFERSIZE;  /* how much to read in each cycle */
        luaL_Buffer b;
        luaL_buffinit(lua, &b);
        for (;;) {
            char *p = luaL_prepbuffsize(&b, rlen);
            //size_t nr = fread(p, sizeof(char), rlen, file);
            auto read_result = file->read(rlen, (uint8_t*)p);        
            luaL_addsize(&b, read_result.bytes());
            if (read_result.bytes() < rlen) break;  /* eof? */
            else if (rlen <= (MAX_SIZE_T / 4))  /* avoid buffers too large */
                rlen *= 2;  /* double buffer size at each iteration */
        }
        luaL_pushresult(&b);  /* close buffer */
    }

    int LuaIOLib::read_number (lua_State *lua, FileHandle *file)
    {
        auto process = proc(lua);
        auto &vfs = process->vfs();
        
        double d;
        if (file->read_number(d).status() == SUCCESS)
        {
            lua_pushnumber(lua, static_cast<lua_Number>(d));
            return 1;
        }
        else 
        {
            lua_pushnil(lua);  /* "result" to be removed */
            return 0;  /* read fails */
        }
    }
    // }}}
    
    // File Write Operatings
    int LuaIOLib::g_write (lua_State *L, FileHandle *file, int arg) 
    {
        int nargs = lua_gettop(L) - arg;
        int status = 1;
        for (; nargs--; arg++) {
            size_t l;
            const char *s = luaL_checklstring(L, arg, &l);
            if (status != 1)
            {
                continue;
            }

            auto result = file->write(l, (const uint8_t *)s);
            if (result.status() != ReturnCode::SUCCESS)
            {
                status = (int)result.status();
                break;
            }
        }
        if (status) return 1;  /* file handle already on stack top */
        else return luaL_fileresult(L, status, NULL);
    }
    // }}}
    
    // }}}
    
    // Directory methods {{{
    const luaL_Reg LuaIOLib::dirlib[] =
    {
        {"read", file_read},
        {"write", file_write},
        {"close", file_close},
        {"lines", file_lines},
        {"__gc", file_gc},
        {"__tostring", file_tostring},
        {NULL, NULL}
    };
    
    void LuaIOLib::add_dir_lib(lua_State *lua)
    {
        luaL_newmetatable(lua, LuaFile::LUA_TABLENAME);
        lua_pushvalue(lua, -1);  /* push metatable */
        lua_setfield(lua, -2, "__index");  /* metatable.__index = metatable */
        luaL_setfuncs(lua, filelib, 0);  /* add file methods to new metatable */
        lua_pop(lua, 1);  /* pop new metatable */
    }
    // }}}
}
