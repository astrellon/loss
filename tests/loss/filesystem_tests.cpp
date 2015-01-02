#include "filesystem_tests.h"

#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>

namespace loss 
{
    namespace tests 
    {

#define CHECK_RESULT(result, message) \
        if (result != loss::SUCCESS) { std::cout << message << ": " << ReturnCodes::desc(result) << "\n"; return false; }

        void Filesystem::run_cases()
        {
            run_case(simple_test);
        }

        std::string Filesystem::suite_name() const
        {
            return std::string("Filesystem");
        }

        bool Filesystem::simple_test()
        {
            VirtualFileSystem vfs;
            RamFileSystem ramfs;

            vfs.root_filesystem(&ramfs);

            CHECK_RESULT(vfs.create_folder("/home"), "Error creating folder");
            CHECK_RESULT(vfs.create_folder("/home/user"), "Error creating folder");

            loss_assert(true);

            return true;
        }
    }
}
