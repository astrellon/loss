#include <iostream>

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
    loss::Kernel kernel(1u);

    auto &vfs = kernel.virtual_file_system();
    auto rootfs = new loss::RamFileSystem();
    vfs.root_filesystem(rootfs);
    {
        std::ifstream input("testout.bin");
        loss::RamFileSystemDeserialise deserialise(input, rootfs);
        deserialise.load();
    }
    
    auto result = kernel.init();
    if (result != loss::SUCCESS)
    {
        std::cout << "Failed to init kernel: " << loss::ReturnCodes::desc(result) << "\n";
        return -1;
    }
        
    std::thread term_thread([] (loss::Kernel *kernel)
    {
        loss::TerminalEmulator renderer;
        loss::FileHandle *handle = nullptr;
        auto &vfs = kernel->virtual_file_system();

        auto result = vfs.open(1u, "/dev/tty0", loss::FileHandle::WRITE | loss::FileHandle::READ, handle);
        renderer.kernel(kernel);
        renderer.file_handle(handle);

        renderer.render();
    }, &kernel);
    
    std::thread kernel_thread([] (loss::Kernel *kernel)
    {
        auto result = kernel->boot();
        if (result != loss::SUCCESS)
        {
            std::cout << "Failed to boot kernel: " << loss::ReturnCodes::desc(result) << "\n";
            return -1;
        }
        return 0;
    }, &kernel);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    kernel.keyboard()->write_string("Hello from keyboard\n");

    term_thread.join();
    kernel_thread.join();

    output_folder(kernel.virtual_file_system(), "/proc");

#ifdef _WIN32
	std::cin.get();
#endif

    kernel.shutdown();
    
    // Shutdown
    if (rootfs != nullptr)
    {
        std::ofstream output("testout.bin");
        loss::RamFileSystemSerialise serialise(output, rootfs);
        serialise.save();
    }
    
    return 0;
}
