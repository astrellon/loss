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

                static void _check_result(const char *file, uint32_t line, loss::ReturnCode result, const std::string &message);
        };

#define check_result(result, message) \
        _check_result(__FILE__, __LINE__, result, message)
    }
}
