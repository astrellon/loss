#include "filesystem_tests.h"

#include <tests/asserts.h>

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

        bool Filesystem::simple_test()
        {
            loss_equals(5, 5);

            return true;
        }
    }
}
