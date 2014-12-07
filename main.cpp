#include <iostream>

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

#include <loss/fs/inode.h>
#include <string>

int main()
{
    /*
    auto lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dostring(lua, "io.write(\"whut\\\n\")");

    lua_close(lua);
    */

    loss::INode root_node;
    loss::INode text_file;
    loss::INode::DataList &text_data = text_file.data();
    for (int i = 'a'; i < 'h'; i++)
    {
        text_data.push_back(i);
    }
    root_node.add_node(std::string("test.txt"), &text_file);

    for (auto iter = root_node.begin_nodes(); iter != root_node.end_nodes(); ++iter)
    {
        auto node = iter->second;
        
        std::cout << "/" << iter->first << "\n";
        std::cout << "- Size: " << node->data_size() << "\n";
        std::cout << "- ";


        for (auto iter2 = node->begin_data(); iter2 != node->end_data(); ++iter2)
        {
            std::cout << (char)(*iter2);
        }
        std::cout << "\n";
    }

    return 0;
}
