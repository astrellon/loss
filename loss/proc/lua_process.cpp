#include "lua_process.h"

#include "../user.h"
#include "../kernel.h"
#include "../lua/lua_wrappers.h"

#include <sstream>

namespace loss
{
    LuaProcess::LuaProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel) :
        IProcess(name, user, id, kernel)
    {
		_lua = luaL_newstate();
        _lua->process = this;
		// We always want the standard libs as they provide basic table manipulation.
		luaL_openlibs(_lua);

        LuaWrappers::add_wrappers(_lua);
    }

	bool LuaProcess::load_string(const std::string &code)
	{
		return !luaL_loadstring(_lua, code.c_str());
	}
    std::string LuaProcess::get_top_error()
    {
        return std::string(lua_tostring(_lua, -1));
    }

    int32_t LuaProcess::run_impl()
    {
        auto result = lua_pcall(_lua, 0, LUA_MULTRET, 0); 
        if (result != LUA_OK)
        {
            std::stringstream ss;
            ss << "\nError running lua " << result << "\n";
            print_stack(ss);
            write_std_out(ss.str());
        }
        else
        {
            write_std_out("\nLua worked\n");
        }
        return 0;
    }
    int32_t LuaProcess::shutdown()
    {
        return 0;
    }
    
	void LuaProcess::print_stack(std::ostream &output)
	{
		auto size = lua_gettop(_lua);
		for (int i = 1; i <= size; i++)
		{
			output << i << ": ";
			
			print_type_value(i, output);

			output << '\n';
		}
	}

	void LuaProcess::print_type_value(int n, std::ostream &output, bool includeType)
	{
		int type = lua_type(_lua, n);
		if (includeType)
		{
			output << ttypename(type) << ' ';
		}
		switch(type) 
		{
		case LUA_TNIL:
			output << "nil";
			break;
		case LUA_TBOOLEAN:
			output << (lua_tobool(_lua, n) ? "true" : "false");
			break;
		case LUA_TSTRING:
			output << lua_tostring(_lua, n);
			break;
		case LUA_TNUMBER:
			output << lua_tonumber(_lua, n);
			break;
		case LUA_TTABLE:
			output << "Table";
			break;
		case LUA_TFUNCTION:
			output << "Function";
			break;
		case LUA_TUSERDATA:
			{
				output << get_type(n);
			}
			break;
		default:
			output << "Unknown";
		}
	}

	const char *LuaProcess::get_type(int n)
	{
		return lua_typename(_lua, n);
	}
}

