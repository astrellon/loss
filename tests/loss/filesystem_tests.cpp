#include "filesystem_tests.h"

#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>

namespace loss 
{
    namespace tests 
    {
        void Filesystem::run_cases()
        {
            run_case(simple_test);
        }

        std::string Filesystem::suite_name() const
        {
            return std::string("Filesystem");
        }

        void Filesystem::simple_test()
        {
            VirtualFileSystem vfs;
            RamFileSystem ramfs;

            vfs.root_filesystem(&ramfs);

            uint32_t size;
            check_result(vfs.entry_size("/", size), "Error getting root");
            loss_equals(0u, size);

            check_result(vfs.create_folder("/home"), "Error creating folder");
            check_result(vfs.entry_size("/", size), "Error getting root");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home", size), "Error getting home");
            loss_equals(0u, size);

            check_result(vfs.create_folder("/home/user"), "Error creating folder");
            check_result(vfs.entry_size("/", size), "Error getting root");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home", size), "Error getting home");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home/user", size), "Error getting user");
            loss_equals(0u, size);

            check_result(vfs.create_file("/home/user/.vimrc"), "Error creating file");
            check_result(vfs.create_file("/home/user/.bashrc"), "Error creating file");
            check_result(vfs.entry_size("/", size), "Error getting root");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home", size), "Error getting home");
            loss_equals(1u, size);
            check_result(vfs.entry_size("/home/user", size), "Error getting user");
            loss_equals(2u, size);

            loss_assert(true);
        }

        void Filesystem::_check_result(const char *file, uint32_t line, loss::ReturnCode result, const std::string &message)
        {
            if (result != loss::SUCCESS)
            {
                std::cout << "Error at: " << file << "[" << line << "]\n- ";
                std::cout << message << ": " << loss::ReturnCodes::name(result) << "\n";
                throw std::runtime_error("");
            }
        }
    }
}
