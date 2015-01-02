#pragma once

#include <tests/test_suite.h>

namespace loss 
{
    namespace tests 
    {
        class Filesystem : public TestSuite
        {
            public:
                virtual void run_cases();
                virtual std::string suite_name() const;

            private:
                void simple_test();

                static void check_result(loss::ReturnCode result, const std::string &message);
        };
    }
}
