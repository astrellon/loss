#include <iostream>

#include <tests/test_suite.h>
#include <tests/loss/filesystem_tests.h>

int main()
{
    loss::tests::Suites suites(std::cout);

    suites.run_suite<loss::tests::Filesystem>();
    return 0;
}
