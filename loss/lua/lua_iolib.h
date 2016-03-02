#pragma once

#include "lua_common.h"

namespace loss
{
    class FileHandle;
    class FolderEntry;

    class LuaFile
    {
        public:
            LuaFile();

            void file(FileHandle *handle);
            FileHandle *file() const;

            static const char *LUA_TABLENAME;
            static int LUA_ID;

        private:
            FileHandle *_file;
    };

    class LuaFolder
    {
        public:
            LuaFolder();

            void folder(FolderEntry *entry);
            FolderEntry *folder() const;

            static const char *LUA_TABLENAME;
            static int LUA_ID;

        private:
            FolderEntry *_folder;
    };

    class LuaIOLib
    {
        public:
            static int add_wrappers(lua_State *lua);

        private:
            // IO Lib
            static const luaL_Reg iolib[];
            static int add_io_lib(lua_State *lua);
            static int io_rename(lua_State *lua);
            static int io_delete(lua_State *lua);
            static int io_open(lua_State *lua);
            static int io_read_dir(lua_State *lua);
            static int io_make_dir(lua_State *lua);

            // File methods
            static const luaL_Reg filelib[];
            static void add_file_lib(lua_State *lua);
            static int file_read(lua_State *lua);
            static int file_write(lua_State *lua);
            static int file_close(lua_State *lua);
            static int file_lines(lua_State *lua);
            static int file_gc(lua_State *lua);
            static int file_tostring(lua_State *lua);

            static LuaFile *new_file(lua_State *lua);

            static int g_read(lua_State *lua, FileHandle *file, int first);
            static int read_line(lua_State *lua, FileHandle *file, int chop);
            static int read_chars (lua_State *lua, FileHandle *file, size_t n);
            static void read_all (lua_State *lua, FileHandle *file);
            static int read_number (lua_State *lua, FileHandle *file);
            
            static int g_write(lua_State *lua, FileHandle *file, int first);
            static int io_readline (lua_State *lua);

            // Folder methods
            static const luaL_Reg dirlib[];
            static void add_dir_lib(lua_State *lua);
    };

}
