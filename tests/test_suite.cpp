#include "test_suite.h"

#include <cstring>

namespace loss
{
    namespace tests
    {
        TestSuite::TestSuite(std::ostream &output) :
            _tests_failed(0),
            _total_tests(0),
            _out(output)
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

        bool TestSuite::run()
        {
            _out << "Starting tests for '" << suite_name() << "'\n";
            before_suite();
            run_cases();

            _out << (_total_tests - _tests_failed) << " of " << _total_tests << " passed ";

            if (_tests_failed > 0)
            {
                _out << "FAIL!";
            }
            _out << "\n";
            after_suite();

            return _tests_failed > 0;
        }

        std::string TestSuite::suite_name() const
        {
            return std::string("Unnamed");
        }

        void TestSuite::disp_error_line(const char *file, uint32_t line) 
        {
            _out << "Error in file " << file << '[' << line << "]";
        }

        void TestSuite::_assert(bool a, const char *file, uint32_t line) 
        {
            if (!a) 
            {
                disp_error_line(file, line);
                _out << "- Assert failed!\n";
                throw std::runtime_error("");
            }
        }
        void TestSuite::_assert(uint32_t a, const char *file, uint32_t line) 
        {
            if (!a) 
            {
                disp_error_line(file, line);
                _out << "- Assert failed!\n";
                throw std::runtime_error("");
            }
        }
        void TestSuite::_assert(void *a, const char *file, uint32_t line) 
        {
            if (!a) 
            {
                disp_error_line(file, line);
                _out << "- Assert failed!\n";
                throw std::runtime_error("");
            }
        }

        void TestSuite::_equals(const char *file, uint32_t line, int32_t expected, int32_t actual, bool not_compare) 
        {
            _simple_compare(expected, actual, not_compare, file, line);
        }
        void TestSuite::_equals(const char *file, uint32_t line, loss::ReturnCode expected, loss::ReturnCode actual, bool not_compare) 
        {
            if (not_compare)
            {
                if (expected == actual)
                {
                    disp_not_error(loss::ReturnCodes::name(expected), file, line);
                    throw std::runtime_error("");
                }
            }
            else
            {
                if (expected != actual)
                {
                    disp_error(loss::ReturnCodes::name(expected), loss::ReturnCodes::name(actual), file, line);
                    throw std::runtime_error("");
                }
            }
        }

        void TestSuite::_equals(const char *file, uint32_t line, uint32_t expected, uint32_t actual, bool not_compare) 
        {
            _simple_compare(expected, actual, not_compare, file, line);
        }
        void TestSuite::_equals(const char *file, uint32_t line, double expected, double actual, bool not_compare) 
        {
            _simple_compare(expected, actual, not_compare, file, line);
        }

        void TestSuite::_equals(const char *file, uint32_t line, const char *expected, const char *actual, bool not_compare) 
        {
            if (not_compare) 
            {
                if (strcmp(expected, actual) == 0) 
                {
                    disp_not_error(expected, file, line);
                    throw std::runtime_error("");
                }
            }
            if (strcmp(expected, actual) != 0) 
            {
                disp_error(expected, actual, file, line);
                throw std::runtime_error("");
            }
        }

        void TestSuite::_equals(const char *file, uint32_t line, double expected, double actual, bool not_compare, double delta) 
        {
            double diff = expected - actual;
            bool equal = diff >= -delta && diff <= delta;
            if (not_compare) 
            {
                if (equal) 
                {
                    disp_not_error(expected, delta, file, line);
                    throw std::runtime_error("");
                }
            }
            if (!equal) 
            {
                disp_error(expected, actual, delta, file, line);
                throw std::runtime_error("");
            }
        }

        void TestSuite::_equals_str(const char *file, uint32_t line, const char *expected, const char *actual, bool not_compare) 
        {
            int cmp = strcmp(expected, actual);
            if (not_compare && cmp == 0) 
            {
                disp_not_error(expected, file, line);
                throw std::runtime_error("");
            }
            else if (!not_compare && cmp != 0) 
            {
                disp_error(expected, actual, file, line);
                throw std::runtime_error("");
            }
        }
        void TestSuite::_equals_str(const char *file, uint32_t line, const char *expected, const std::string &actual, bool not_compare) {
            int cmp = strcmp(expected, actual.c_str());
            if (not_compare && cmp == 0) 
            {
                disp_not_error(expected, file, line);
                throw std::runtime_error("");
            }
            else if (!not_compare && cmp != 0) 
            {
                disp_error(expected, actual.c_str(), file, line);
                throw std::runtime_error("");
            }
        }
        
        Suites::Suites(std::ostream &output) :
            _out(output)
        {

        }
        const std::vector<std::string> &Suites::passed_tests() const
        {
            return _passed_tests;
        }
        const std::vector<std::string> &Suites::failed_tests() const
        {
            return _failed_tests;
        }

    }
}
