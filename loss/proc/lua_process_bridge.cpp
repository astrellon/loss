#include "lua_process_bridge.h"

#include "lua_process.h"
#include "../lua/lua_common.h"

void check_lua_process_yielding(void *lua)
{
    auto proc = loss::proc((lua_State *)lua);
    proc->check_for_yield();
}
