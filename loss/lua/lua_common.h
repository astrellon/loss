#pragma once 

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

#define lua_tofloat(L,i)	static_cast<float>(lua_tonumber(L,i))
#define lua_tobool(L,i)		(lua_toboolean(L,i) > 0)
#define lua_isnum(L,i)		(lua_type(L,i) == LUA_TNUMBER)
#define lua_isstr(L,i)		(lua_type(L,i) == LUA_TSTRING)
#define lua_isbool(L,i)		(lua_type(L,i) == LUA_TBOOLEAN)
#define lua_first(L)		lua_pushvalue(L,1); return 1;

namespace loss
{
    class LuaProcess;

    static inline LuaProcess *proc(lua_State *lua)
    {
        return reinterpret_cast<LuaProcess *>(lua->process);
    }
    
    /**
     * Helper struct for storing Lua userdata.
     * Contains an id which identifies the class and a pointer to
     * the current instance.
     */
    typedef struct 
    {
        int id;
        void *ptr;
    } LuaUData;

    /**
     * Allocates userdata memory in Lua.
     * Allocates enough space for a LuaUData struct.
     *
     * @param lua The lua_State to allocate the memory on.
     * @param obj The object to allocate the memory for.
     */
    template <class T>
    static inline void new_userdata(lua_State *lua, T *obj)
    {
        LuaUData *udata = reinterpret_cast<LuaUData *>(
            lua_newuserdata(lua, sizeof(LuaUData)));
        udata->id = T::LUA_ID;
        udata->ptr = obj;
    }

    /**
     * Attempts to cast the parameter at index n to the given class T.
     * This only works if the memory was allocated by the newUData function,
     * or if the userdata is structurally the same as a LuaUData struct.
     *
     * @param lua The lua_State to work on.
     * @param n The stack index to the value.
     * @return A non-nullptr value if the id's match
     */
    template <class T>
    static inline T *cast_userdata(lua_State *lua, int n)
    {
        if (lua_type(lua, n) == LUA_TUSERDATA)
        {
            LuaUData *udata = reinterpret_cast<LuaUData *>(lua_touserdata(lua, n));
            if (udata->id == T::LUA_ID)
            {
                return reinterpret_cast<T *>(udata->ptr);
            }
        }
        return nullptr;
    }

    static inline int get_userdata_type(lua_State *lua, int n)
    {
        if (!lua_isuserdata(lua, n))
        {
            return -1;
        }
        int *data = reinterpret_cast<int *>(lua_touserdata(lua, n));
        return data[0];
    }

    /**
     * Wraps an object instance into Lua.
     * This requires that this class have the static consts
     * - int LUA_ID
     * - const char *LUA_TABLENAME
     * These are both used to identify the class type at a later date
     * and for applying which metatable.
     *
     * Once the object is wrapped it is left on top of the Lua stack.
     *
     * @param lua The lua_State to work on.
     * @param object The object to wrap.
     */
    template <class T>
    static inline void wrap_object(lua_State *lua, T *object)
    {
        new_userdata<T>(lua, object);

        luaL_getmetatable(lua, T::LUA_TABLENAME);
        lua_setmetatable(lua, -2);
    }
}
