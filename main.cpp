#include <iostream>

extern "C" 
{ 
#	include <lua/src/lua.h>
#	include <lua/src/lauxlib.h>
#	include <lua/src/lualib.h>
#	include <lua/src/lstate.h>
} 

#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <sstream>
#include <chrono>
#include <condition_variable>

#include <loss/kernel.h>
#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>
#include <loss/return_codes.h>
#include <loss/fs/path.h>
#include <loss/fs/ram_filesystem_drive.h>
#include <loss/terminal_emulator.h>
#include <loss/fs/stream_device.h>

void output_folder(loss::VirtualFileSystem &vfs, const std::string &name)
{
    std::cout << "Output: " << name << "\n";
    auto folder = new loss::FolderEntry();
    auto folder_result = vfs.read_folder(name, *folder);
    if (folder_result == loss::SUCCESS)
    {
        //for (auto iter = folder->begin_folders(); iter != folder->end_folders(); ++iter)
        for (auto &iter : *folder)
        {
            std::cout << "Entry: " << iter.first << "/\n";
        }

        /*
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
        */
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

    loss::Kernel kernel(1u);
    auto &vfs = kernel.virtual_file_system();
    auto ramfs = new loss::RamFileSystem();

    vfs.root_filesystem(ramfs);

    auto device = new loss::StreamDevice();
    vfs.create_char_device("/tty0", device);

    vfs.create_file("/what.txt");

    loss::FileHandle *handle = nullptr;
    auto result = vfs.open(1u, "/tty0", loss::FileHandle::READ | loss::FileHandle::WRITE, handle);
    vfs.write_string(handle, 0, "hellor");

    loss::TerminalEmulator renderer;
    renderer.kernel(&kernel);
    renderer.file_handle(handle);
    
    std::thread write_thread([] (loss::Kernel &kernel)
    {
        auto n = 0u;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::stringstream ss;
            ss << "Writing: " << n;
            kernel.virtual_file_system().write_string("/tty0", 0, ss.str());
            n++;
        }
    }, std::ref(kernel));

    renderer.render();

    vfs.close(handle);

    /*
    vfs.write_string("/tty0", 0, "hellor");

    std::stringstream output;
    vfs.read_stream("/tty0", 0, 10, output);

    std::cout << "Read from tty0: " << output.str() << "\n";
    */
    /*
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

    result = vfs.register_file_system(ramfs2);
    if (result != loss::SUCCESS)
    {
        std::cout << "Failed to register_file_system: " << loss::ReturnCodes::desc(result) << "\n";
    }

    result = vfs.mount("/home/alan/other_fs", ramfs2);
    if (result != loss::SUCCESS)
    {
        std::cout << "Error mounting: " << loss::ReturnCodes::desc(result) << "\n";
    }

    result = vfs.create_file("/home/alan/other_fs/other.txt");
    if (result != loss::SUCCESS)
    {
        std::cout << "Error creating file in mounted: " << loss::ReturnCodes::desc(result) << "\n";
    }

    ioresult = vfs.write_string("/home/alan/other_fs/other.txt", 0, "hurr durr");
    if (ioresult.status() != loss::SUCCESS)
    {
        std::cout << "Error writing to other: " << loss::ReturnCodes::desc(ioresult.status()) << "\n";
    }
    */
    
    output_folder(vfs, "/");
    output_folder(vfs, "/home");
    output_folder(vfs, "/home/alan");
    output_folder(vfs, "/home/alan/other_fs");
                
    {
        std::ofstream output("testout.bin");
        auto serialise = loss::RamFileSystemSerialise(output, ramfs);
        serialise.save();
    }

    /*
    loss::TTY tty;
    tty.size(80, 24);
    tty.put_string("Hello");
    loss::TTYRenderer renderer;
    renderer.tty(&tty);
    renderer.render();
    */

    /*
    loss::StreamDevice device;
    device.write_string(std::string("Hello thar"));

    std::cout << "Size: " << device.size() << "\n";

    std::stringstream read_buff;
    device.read_string(read_buff);

    std::cout << "READ THE THING >" << read_buff.str() << "<\n";
    std::cout << "Size: " << device.size() << "\n";
    
    std::stringstream read_buff2;
    device.read_string(read_buff2);
	
    std::cout << "READ THE THING >" << read_buff2.str() << "<\n";
    std::cout << "Size: " << device.size() << "\n";
    */

    /*
    loss::StreamDevice device;
#define NUM_THREADS 50
    std::thread write_threads[NUM_THREADS];
    for (auto i = 0; i < NUM_THREADS; i++)
    {
        write_threads[i] = std::thread([] (int n, loss::StreamDevice &d)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50 * n));
            std::stringstream ss;
            ss << "Thread: " << n << "!\n";
            d.write_string(ss.str());
        }, i, std::ref(device));
    }

    auto done = false;
    std::thread read_thread([] (loss::StreamDevice &d, bool &done)
    {
        std::stringstream buff;
        while (!done)
        {
            buff.str("");
            d.read_string(buff);

            //if (buff.str().size() > 0u)
            {
                std::cout << "-- >" << buff.str() << "< --\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }, std::ref(device), std::ref(done));

    for (auto i = 0; i < NUM_THREADS; i++)
    {
        write_threads[i].join();
    }

    std::cout << "***\n";
    //done = true;
    read_thread.join();

    std::cout << " - DONE\n";
    */

#ifdef _WIN32
	std::cin.get();
#endif

    return 0;
}
