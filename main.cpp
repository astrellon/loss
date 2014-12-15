#include <iostream>

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

#include <loss/fs/virtual_filesystem.h>
#include <loss/return_codes.h>
#include <string>

int main()
{
    /*
    auto lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dostring(lua, "io.write(\"whut\\\n\")");

    lua_close(lua);
    */

    /*
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
    */

    loss::VirtualFileSystem fs;

    loss::FolderEntry root;
    auto status = fs.getdir("/", &root);
    if (status != loss::SUCCESS)
    {
        std::cout << "Error getting directory: " << loss::ReturnCodes::desc(status) << "\n";
    }

    for (auto iter = root.begin_folders(); iter != root.end_folders(); ++iter)
    {
        std::cout << "Dir: " << iter->first << "\n";
    }
    for (auto iter = root.begin_files(); iter != root.end_files(); ++iter)
    {
        std::cout << "File: " << iter->first << " | " << iter->second->size() << "\n";
        std::cout << "- ";
        uint8_t temp[256];
        auto read_result = fs.read(iter->first, 0, 256, temp);
        if (read_result.status() != loss::SUCCESS)
        {
            std::cout << "Error reading file: " << loss::ReturnCodes::desc(read_result.status()) << "\n";
        }
        else
        {
            for (uint32_t i = 0; i < read_result.bytes(); i++)
            {
                std::cout << (char)temp[i];
            }
            std::cout << "\n";
        }
    }
	
#ifdef _WIN32
	std::cin.get();
#endif

    return 0;
}
