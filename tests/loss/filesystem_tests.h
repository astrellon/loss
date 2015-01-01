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
                bool simple_test();
        };
    }
}
