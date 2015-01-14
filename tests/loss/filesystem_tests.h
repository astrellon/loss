#pragma once

#include <iostream>
#include <tests/test_suite.h>

namespace loss 
{
    namespace tests 
    {
        class Filesystem : public TestSuite
        {
            public:
                Filesystem(std::ostream &output);

                virtual void run_cases();
                virtual std::string suite_name() const;

            private:
                void simple_test();
                void filehandle_test();

                static void _check_result(const char *file, uint32_t line, ReturnCode result, const std::string &message, ReturnCode expected);
        };

#define check_result(result, expected, message) \
        _check_result(__FILE__, __LINE__, result, message, expected)
    }
}
