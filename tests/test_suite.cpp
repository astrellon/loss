#include "test_suite.h"

namespace loss
{
    namespace tests
    {
        TestSuite::TestSuite() :
            _tests_failed(0),
            _total_tests(0)
        {

        }
        TestSuite::~TestSuite()
        {

        }

        void TestSuite::before_suite()
        {

        }
        void TestSuite::after_suite()
        {

        }
        void TestSuite::before_case()
        {

        }
        void TestSuite::after_case()
        {

        }

        void TestSuite::on_error()
        {

        }

        void TestSuite::run()
        {
            std::cout << "Starting tests for '" << suite_name() << "'\n";
            before_suite();
            run_cases();

            try
            {

            }
            catch (...)
            {

            }
            std::cout << (_total_tests - _tests_failed) << " of " << _total_tests << " passed ";

            if (_tests_failed > 0)
            {
                std::cout << "FAIL!";
                failed_test(suite_name());
            }
            std::cout << "\n";
            after_suite();
        }

        std::string TestSuite::suite_name() const
        {
            return std::string("Unnamed");
        }

        void TestSuite::failed_test(const std::string &name)
        {
            s_failed_tests.push_back(name);
        }
        const std::vector<std::string> &TestSuite::failed_tests()
        {
            return s_failed_tests;
        }

        std::vector<std::string> TestSuite::s_failed_tests;

    }
}
