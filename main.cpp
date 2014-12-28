#include <iostream>

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>
#include <loss/return_codes.h>
#include <loss/fs/path.h>
#include <string>

void output_folder(loss::VirtualFileSystem &vfs, const std::string &name)
{
    std::cout << "Output: " << name << "\n";
    auto folder = new loss::FolderEntry();
    auto folder_result = vfs.read_folder(name, folder);
    if (folder_result == loss::SUCCESS)
    {
        for (auto iter = folder->begin_folders(); iter != folder->end_folders(); ++iter)
        {
            std::cout << "Folder: " << iter->first << "/\n";
        }

        for (auto iter = folder->begin_files(); iter != folder->end_files(); ++iter)
        {
            std::cout << "File: " << iter->first << "\n";
            uint8_t buffer[256];

            auto ioresult = vfs.read(iter->second, 0, 255, buffer);
            if (ioresult.status() != loss::SUCCESS)
            {
                std::cout << "Error reading: " << loss::ReturnCodes::desc(ioresult.status()) << "\n";
            }
            else
            {
                std::cout << "Buffer size: " << ioresult.bytes() << "\n";
                for (uint32_t i = 0; i < ioresult.bytes(); i++)
                {
                    std::cout << (char)buffer[i];
                }
                std::cout << "\n";
            }
        }
    }
    delete folder;

    std::cout << "\n";
}

int main()
{
    /*
    auto lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dostring(lua, "io.write(\"whut\\\n\")");

    lua_close(lua);
    */

    loss::VirtualFileSystem vfs;
    loss::RamFileSystem ramfs;
    loss::RamFileSystem ramfs2;

    vfs.root_filesystem(&ramfs);
    
    auto result = vfs.create_folder("/home");
    if (result != loss::SUCCESS)
    {
        std::cout << "Error creating folder: " << loss::ReturnCodes::desc(result) << "\n";
    }
    
    result = vfs.create_folder("/home/alan");
    if (result != loss::SUCCESS)
    {
        std::cout << "Error creating folder: " << loss::ReturnCodes::desc(result) << "\n";
    }

    result = vfs.create_file("/home/alan/test.txt");
    if (result != loss::SUCCESS)
    {
        std::cout << "Error creating file: " << loss::ReturnCodes::desc(result) << "\n";
    }

    auto ioresult = vfs.write_string("/home/alan/test.txt", 0, "whut up brah");
    if (ioresult.status() != loss::SUCCESS)
    {
        std::cout << "Error writing: " << loss::ReturnCodes::desc(ioresult.status()) << "\n";
    }


    result = vfs.mount("/home/alan/other_fs", &ramfs2);
    if (result != loss::SUCCESS)
    {
        std::cout << "Error mounting: " << loss::ReturnCodes::desc(result) << "\n";
    }
    
    output_folder(vfs, "/");
    output_folder(vfs, "/home");
    output_folder(vfs, "/home/alan");
	
#ifdef _WIN32
	std::cin.get();
#endif

    return 0;
}
