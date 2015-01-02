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

            check_result(vfs.create_folder("/home"), "Error creating folder");
            check_result(vfs.create_folder("/home/user"), "Error creating folder");

            loss_assert(true);
        }

        void Filesystem::check_result(loss::ReturnCode result, const std::string &message)
        {
            if (result != loss::SUCCESS)
            {
                std::cout << message << ": " << loss::ReturnCodes::name(result) << "\n";
                throw std::runtime_error("");
            }
        }
    }
}
