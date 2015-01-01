#include "asserts.h"

#include <cstring>

namespace loss
{
    namespace tests
    {
        void Asserts::disp_error_line(const char *file, int line) 
        {
            std::cout << "Error in file " << file << '[' << line << "]";

        }

        bool Asserts::_assert(bool a, const char *file, int line) 
        {
            if (!a) 
            {
                disp_error_line(file, line);
                std::cout << "- Assert failed!\n";
                return false;
            }
            return true;
        }
        bool Asserts::_assert(int a, const char *file, int line) 
        {
            if (!a) 
            {
                disp_error_line(file, line);
                std::cout << "- Assert failed!\n";
                return false;
            }
            return true;
        }
        bool Asserts::_assert(void *a, const char *file, int line) 
        {
            if (!a) 
            {
                disp_error_line(file, line);
                std::cout << "- Assert failed!\n";
                return false;
            }
            return true;
        }

        bool Asserts::_equals(const char *file, unsigned int line, const int &expected, const int &actual, bool not_compare) 
        {
            _simple_compare(expected, actual, not_compare, file, line);
        }
        bool Asserts::_equals(const char *file, unsigned int line, loss::ReturnCode expected, loss::ReturnCode actual, bool not_compare) 
        {
            //_simple_compare(expected, actual, not_compare, file, line);
            if (not_compare)
            {
                if (expected == actual)
                {
                    disp_not_error(loss::ReturnCodes::name(expected), file, line);
                    return false;
                }
            }
            else
            {
                if (expected != actual)
                {
                    disp_error(loss::ReturnCodes::name(expected), loss::ReturnCodes::name(actual), file, line);
                    return false;
                }
            }
            return true;
        }

        bool Asserts::_equals(const char *file, unsigned int line, const unsigned int &expected, const unsigned int &actual, bool not_compare) 
        {
            _simple_compare(expected, actual, not_compare, file, line);
        }
        bool Asserts::_equals(const char *file, unsigned int line, const double &expected, const double &actual, bool not_compare) 
        {
            _simple_compare(expected, actual, not_compare, file, line);
        }

        bool Asserts::_equals(const char *file, unsigned int line, const char *expected, const char *actual, bool not_compare) 
        {
            if (not_compare) 
            {
                if (strcmp(expected, actual) == 0) 
                {
                    disp_not_error(expected, file, line);
                    return false;
                }
                return true;
            }
            if (strcmp(expected, actual) != 0) 
            {
                disp_error(expected, actual, file, line);
                return false;
            }
            return true;
        }

        bool Asserts::_equals(const char *file, unsigned int line, const double &expected, const double &actual, bool not_compare, double delta) 
        {
            double diff = expected - actual;
            bool equal = diff >= -delta && diff <= delta;
            if (not_compare) 
            {
                if (equal) 
                {
                    disp_not_error(expected, delta, file, line);
                    return false;
                }
                return true;
            }
            if (!equal) 
            {
                disp_error(expected, actual, delta, file, line);
                return false;
            }
            return true;
        }

        bool Asserts::_equals_str(const char *file, unsigned int line, const char *expected, const char *actual, bool not_compare) 
        {
            int cmp = strcmp(expected, actual);
            if (not_compare && cmp == 0) 
            {
                disp_not_error(expected, file, line);
                return false;
            }
            else if (!not_compare && cmp != 0) 
            {
                disp_error(expected, actual, file, line);
                return false;
            }
            return true;
        }
        bool Asserts::_equals_str(const char *file, unsigned int line, const char *expected, const std::string &actual, bool not_compare) {
            int cmp = strcmp(expected, actual.c_str());
            if (not_compare && cmp == 0) 
            {
                disp_not_error(expected, file, line);
                return false;
            }
            else if (!not_compare && cmp != 0) 
            {
                disp_error(expected, actual.c_str(), file, line);
                return false;
            }
            return true;
        }
    }
}
