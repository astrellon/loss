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

/*
void output_folder(loss::IFileSystem &fs, const std::string &name)
{
    std::cout << "Outputting: " << name << "\n";
    loss::FolderEntry root(&fs);
    auto status = fs.getdir(name, &root);
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
}
*/

int main()
{
    /*
    auto lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dostring(lua, "io.write(\"whut\\\n\")");

    lua_close(lua);
    */

    /*
    loss::RamFileSystem fs;

    auto write_result = fs.write_string(std::string("/test.txt"), 0, std::string("Whut urp"));
    if (write_result.status() != loss::SUCCESS)
    {
        std::cout << "Failed to write to test file: " << loss::ReturnCodes::desc(write_result.status()) << "\n";
    }
    write_result = fs.write_string(std::string("/test2.txt"), 0, std::string("Nothing much really"));

    auto create_result = fs.create_folder("/home");
    if (create_result != loss::SUCCESS)
    {
        std::cout << "Error creating folder: " << loss::ReturnCodes::desc(create_result) << "\n";
    }
    create_result = fs.create_folder("/home/melli");
    if (create_result != loss::SUCCESS)
    {
        std::cout << "Error creating folder: " << loss::ReturnCodes::desc(create_result) << "\n";
    }

    output_folder(fs, "/");
    output_folder(fs, "/home");
    */

    loss::VirtualFileSystem vfs;
    loss::RamFileSystem ramfs;
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

    uint8_t buffer[256];
    auto ioresult = vfs.read("/home/alan/test.txt", 0, 255, buffer);
    if (ioresult.status() != loss::SUCCESS)
    {
        std::cout << "Error reading: " << loss::ReturnCodes::desc(ioresult.status()) << "\n";
    }

    result = vfs.create_file("/home/alan/test.txt");
    if (result != loss::SUCCESS)
    {
        std::cout << "Error creating file: " << loss::ReturnCodes::desc(result) << "\n";
    }

    ioresult = vfs.write_string("/home/alan/test.txt", 0, "whut up brah");
    if (ioresult.status() != loss::SUCCESS)
    {
        std::cout << "Error writing: " << loss::ReturnCodes::desc(ioresult.status()) << "\n";
    }

    ioresult = vfs.read("/home/alan/test.txt", 0, 255, buffer);
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
	
#ifdef _WIN32
	std::cin.get();
#endif

    return 0;
}
