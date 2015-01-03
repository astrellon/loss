#include <iostream>

#include <tests/test_suite.h>
#include <tests/loss/filesystem_tests.h>

int main()
{
    loss::tests::Suites suites(std::cout);

    suites.run_suite<loss::tests::Filesystem>();

#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}
