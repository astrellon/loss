#include <iostream>

#include <tests/test_suite.h>
#include <tests/loss/filesystem_tests.h>

int main()
{
    uint32_t suite_count = 0;

    run_suite(loss::tests::Filesystem);
    return 0;
}
