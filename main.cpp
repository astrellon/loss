#include <iostream>

#include <string>
#include <fstream>
#include <thread>
#include <sstream>
#include <chrono>

#ifdef _WIN32
extern "C"
{
#   include <pdcurses/curses.h>
}
#else
extern "C"
{
#   include <ncurses.h>
}
#endif

#include <loss/kernel.h>
#include <loss/kernel_manager.h>
#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>
#include <loss/return_codes.h>
#include <loss/fs/path.h>
#include <loss/fs/ram_filesystem_drive.h>
#include <loss/kernel_stream.h>

#include <loss/proto/display_serialiser.h>

void output_folder(loss::VirtualFileSystem &vfs, const std::string &name)
{
    std::cout << "Output: " << name << "\n";
    auto folder = new loss::FolderEntry();
    auto folder_result = vfs.read_folder(1u, name, *folder);
    if (folder_result == loss::SUCCESS)
    {
        for (auto &iter : *folder)
        {
            std::cout << "Entry: " << iter.first << "/\n";
        }
    }
    delete folder;

    std::cout << "\n";
}

int main(int argc, char **argv)
{
    auto kernel = loss::KernelManager::create_new_kernel();

    std::string inputFile = "testout.bin";
    if (argc > 1)
    {
        inputFile = std::string(argv[1]);
        std::cout << "Using inputFile: " << inputFile << std::endl;
    }

    auto &vfs = kernel->virtual_file_system();
    auto rootfs = new loss::RamFileSystem();
    vfs.root_filesystem(rootfs);
    {
        std::ifstream input(inputFile);
        loss::RamFileSystemDeserialise deserialise(input, rootfs);
        deserialise.load();
    }
    
    auto result = kernel->init();
    if (result != loss::SUCCESS)
    {
        std::cout << "Failed to init kernel: " << loss::ReturnCodes::desc(result) << "\n";
        return -1;
    }
    
    std::thread kernel_thread([] (loss::Kernel *kernel)
    {
        auto result = kernel->boot();
        if (result != loss::SUCCESS)
        {
            std::cout << "Failed to boot kernel: " << loss::ReturnCodes::desc(result) << " (" << result << ")\n";
            return -1;
        }

        return 0;
    }, kernel);

    std::thread keyboard_thread([] (loss::Kernel *kernel)
    {
        while (kernel->is_running())
        {
            std::string input;
            std::cin >> input;
            if (input == "end")
            {
                kernel->shutdown();
                break;
            }
            input += "\n";
            kernel->keyboard()->write_threaded_string(0, input);
        }
        std::cout << "Keyboard thread done\n";
    }, kernel);

    kernel_thread.join();
    keyboard_thread.join();

    output_folder(vfs, "/proc");

#ifdef _WIN32
	std::cin.get();
#endif
    
    // Shutdown
    if (rootfs != nullptr)
    {
        std::ofstream output("testout.bin");
        loss::RamFileSystemSerialise serialise(output, rootfs);
        serialise.save();
        std::cout << "Saved hdd back to testout.bin\n";
    }
    
    return 0;
}
