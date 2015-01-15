#include "filesystem_tests.h"

#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>
#include <loss/fs/ram_filesystem_drive.h>

#include <fstream>

namespace loss 
{
    namespace tests 
    {
        Filesystem::Filesystem(std::ostream &output) :
            TestSuite(output)
        {

        }

        void Filesystem::run_cases()
        {
            run_case(simple_test);
            run_case(filehandle_test);
        }

        std::string Filesystem::suite_name() const
        {
            return std::string("Filesystem");
        }

        void Filesystem::simple_test()
        {
            VirtualFileSystem vfs;
            RamFileSystem *ramfs = new RamFileSystem();

            vfs.root_filesystem(ramfs);

            uint32_t size;
            check_result(vfs.entry_size("/", size), SUCCESS, "Error getting root");
            loss_equals(0u, size);

            check_result(vfs.create_folder("/home"), SUCCESS, "Error creating folder");
            check_result(vfs.entry_size("/", size), SUCCESS, "Error getting root");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home", size), SUCCESS, "Error getting home");
            loss_equals(0u, size);

            check_result(vfs.create_folder("/home/user"), SUCCESS, "Error creating folder");
            check_result(vfs.entry_size("/", size), SUCCESS, "Error getting root");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home", size), SUCCESS, "Error getting home");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home/user", size), SUCCESS, "Error getting user");
            loss_equals(0u, size);

            check_result(vfs.create_file("/home/user/.vimrc"), SUCCESS, "Error creating file");
            check_result(vfs.create_file("/home/user/.bashrc"), SUCCESS, "Error creating file");
            check_result(vfs.create_folder("/home/user/.vim"), SUCCESS, "Error creating folder");

            check_result(vfs.entry_size("/", size), SUCCESS, "Error getting root");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home", size), SUCCESS, "Error getting home");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home/user", size), SUCCESS, "Error getting user");
            loss_equals(3u, size);
            check_result(vfs.entry_size("/home/user/.vim", size), SUCCESS, "Error getting user");
            loss_equals(0u, size);

            check_result(vfs.remove_entry("/home/user"), FOLDER_NOT_EMPTY, "Deleted not empty folder");
            check_result(vfs.remove_entry("/home/user/.vim"), SUCCESS, "Error deleting empty folder");

            check_result(vfs.entry_size("/home/user", size), SUCCESS, "Error getting user");
            loss_equals(2u, size);
            check_result(vfs.entry_size("/home/user/.vim", size), ENTRY_NOT_FOUND, "Found deleted entry");

            check_result(vfs.entry_size("/home/user/.vimrc", size), SUCCESS, "Error getting .vimrc size");
            loss_equals(0u, size);

            auto write_result = vfs.write_string("/home/user/.vimrc", 0, "set autoindent");
            check_result(write_result.status(), SUCCESS, "Error writing to .vimrc");
            loss_equals(write_result.bytes(), 14u);

            std::stringstream ss;
            auto read_result = vfs.read_stream("/home/user/.vimrc", 0, 20u, ss);
            check_result(read_result.status(), SUCCESS, "Error reading .vimrc");
            loss_equals(read_result.bytes(), 14u);

            loss_equals_str("set autoindent", ss.str().c_str());

            {
                std::ofstream output("testout.bin");
                auto serialise = RamFileSystemSerialise(output, ramfs);
                serialise.save();
            }

            {
                std::unique_ptr<RamFileSystem> tempfs(new RamFileSystem());
                std::ifstream input("testout.bin");
                auto deserialise = RamFileSystemDeserialise(input, tempfs.get());
                deserialise.load();
                
                std::ofstream output("testout2.bin");
                auto serialise = RamFileSystemSerialise(output, tempfs.get());
                serialise.save();
            }
        }
        
        void Filesystem::filehandle_test()
        {
            VirtualFileSystem vfs;
            RamFileSystem *ramfs = new RamFileSystem();

            vfs.root_filesystem(ramfs);
            check_result(vfs.create_file("/test.log"), SUCCESS, "Error creating file");

            FileHandle *file = nullptr;
            check_result(vfs.open(1, "/whut.whut", FileHandle::READ | FileHandle::WRITE, file), ENTRY_NOT_FOUND, "Found non-existent file");
            loss_assert(file == nullptr);

            check_result(vfs.open(1, "/test.log", FileHandle::READ | FileHandle::WRITE,file), SUCCESS, "Error opening file handle");
            loss_assert(file != nullptr);

            auto ioresult = vfs.write_string(file, 0, std::string("Hello thar"));
            check_result(ioresult.status(), SUCCESS, "Error writing to file");
            loss_equals(10u, ioresult.bytes());

            check_result(vfs.close(file), SUCCESS, "Error closing file handle");

            check_result(vfs.open(1, "/whut.whut", FileHandle::WRITE, file), SUCCESS, "Error opening file handle");
            loss_assert(file != nullptr);
        }

        void Filesystem::_check_result(const char *file, uint32_t line, loss::ReturnCode result, const std::string &message, loss::ReturnCode expected)
        {
            if (result != expected)
            {
                std::cout << "Error at: " << file << "[" << line << "]\n- "
                    << message << ": " << ReturnCodes::name(result) << "\n"
                    << "- Expected: " << ReturnCodes::name(expected) << "\n";

                throw std::runtime_error("");
            }
        }
    }
}
