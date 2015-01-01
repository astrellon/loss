#pragma once

#define IN_TEST_CASE

#include <iostream>
#include <string>
#include <sstream>

#include <loss/return_codes.h>

namespace loss
{
    namespace tests
    {
        class Asserts
        {
            public:
                static void disp_error_line(const char *file, int line);
                static bool _assert(bool a, const char *file, int line);
                static bool _assert(int a, const char *file, int line);
                static bool _assert(void *a, const char *file, int line);

                template <class T>
                    static void disp_error(const T &expected, const T &actual, const char *file, unsigned int line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "\n- Expected: >" << expected << "<\n- Actual: >" << actual << "<\n";
                    }

                template <class T>
                    static void disp_not_error(const T &expected, const char *file, unsigned int line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "- Did not expect: " << expected << "\n";
                    }

                template <class T>
                    static void disp_error(const T &expected, const T &actual, double delta, const char *file, unsigned int line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "- Expected: " << expected << "\n- Actual  : " << actual << "\n- Delta   : " << delta << "\n";
                    }

                template <class T>
                    static void disp_not_error(const T &expected, double delta, const char *file, unsigned int line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "- Did not expect: " << expected << "\n- Delta   : " << delta << "\n";
                    }

                template <class T>
                    static void concat_array(const T *array, int n, std::stringstream &ss)
                    {
                        ss << '[';
                        for (int i = 0; i < n; i++) 
                        {
                            if (i > 0) 
                            {
                                ss << ", ";
                            }
                            ss << array[i];
                        }
                        ss << ']';
                    }

                template <class T>
                    static void disp_array_error(const T *expected, const T *actual, int n, int errn, double delta, const char *file, unsigned int line)
                    {
                        disp_error_line(file, line);
                        std::stringstream ss;
                        ss <<   "- Expected: ";
                        concat_array(expected, n, ss);
                        ss << "\n- Actual  : ";
                        concat_array(actual, n, ss);
                        ss << "\n- Delta   : " << delta
                            << "\n- Index   : " << errn;
                        std::cout << ss;
                    }
                template <class T>
                    static void disp_array_not_error(const T *expected, int n, int errn, double delta, const char *file, unsigned int line)
                    {
                        disp_error_line(file, line);
                        std::stringstream ss;
                        ss << "- Did not expect: ";
                        concat_array(expected, n, ss);
                        ss << "\n- Delta: " << delta
                            << "\n- Index: " << errn;
                        std::cout << ss;
                    }

#define _simple_compare(e, a, r, f, l)	\
                if (r) {	\
                    if (e == a) { Asserts::disp_not_error(e, f, l); return false; }	return true;	\
                } else {	\
                    if (e != a) { Asserts::disp_error(e, a, f, l);	return false; }	return true; }

                static bool _equals(const char *file, unsigned int line, const int &expected, const int &actual, bool not_compare);
                static bool _equals(const char *file, unsigned int line, loss::ReturnCode expected, loss::ReturnCode actual, bool not_compare);
                static bool _equals(const char *file, unsigned int line, const unsigned int &expected, const unsigned int &actual, bool not_compare);
                static bool _equals(const char *file, unsigned int line, const double &expected, const double &actual, bool not_compare);
                static bool _equals(const char *file, unsigned int line, const char *expected, const char *actual, bool not_compare);
                static bool _equals(const char *file, unsigned int line, const double &expected, const double &actual, bool not_compare, double delta=0.00001);

                static bool _equals_str(const char *file, unsigned int line, const char *expected, const char *actual, bool not_compare);
                static bool _equals_str(const char *file, unsigned int line, const char *expected, const std::string &actual, bool not_compare);

                template <class T>
                    static bool _equals_array(const char *file, unsigned int line, const T *expected, const T *actual, bool not_compare, double delta=0.00001, int n=16)
                    {
                        bool equal = true;
                        int i = 0;
                        for (; i < n; i++) 
                        {
                            double diff = expected[i] - actual[i];
                            if (diff < -delta || diff > delta) 
                            {
                                equal = false;
                                break;
                            }
                        }
                        if (not_compare && equal) 
                        {
                            disp_array_not_error(expected, n, i, delta, file, line);
                            return false;
                        }
                        else if (!not_compare && !equal) 
                        {
                            disp_array_error(expected, actual, n, i, delta, file, line);
                            return false;
                        }
                        return true;
                    }
        };
    }
}

#ifdef IN_TEST_CASE
#	define assert(a) \
	if (!Asserts::_assert(a, __FILE__, __LINE__)) { return false; }

//  General equals
#	define loss_equals(expected, actual) \
	if (!Asserts::_equals(__FILE__, __LINE__, expected, actual, false)) { return false; }
#	define loss_equals_delta(expected, actual, delta) \
	if (!Asserts::_equals(__FILE__, __LINE__, expected, actual, false, delta)) { return false; }

#	define loss_not_equals(expected, actual) \
	if (!Asserts::_equals(__FILE__, __LINE__, expected, actual, true)) { return false; }
#	define loss_not_equalsDelta(expected, actual, delta) \
	if (!Asserts::_equals(__FILE__, __LINE__, expected, actual, true, delta)) { return false; }

//  String equals
#	define loss_equals_str(expected, actual)	\
	if (!Asserts::_equals_str(__FILE__, __LINE__, expected, actual, false)) { return false; }
#	define not_equalsStr(expected, actual)	\
	if (!Asserts::_equals_str(__FILE__, __LINE__, expected, actual, true)) { return false; }

//  _array equals
#   define loss_equals_array(expected, actual, n)  \
    if (!Asserts::_equals_array(__FILE__, __LINE__, expected, actual, false, 0.00001, n)) { return false; }
#   define loss_not_equals_array(expected, actual, n)  \
    if (!Asserts::_equals_array(__FILE__, __LINE__, expected, actual, true, 0.00001, n)) { return false; }

#else
#	define assert(a) _assert(a, __FILE__, __LINE__)

#	define loss_equals(expected, actual) _equals(__FILE__, __LINE__, expected, actual, false)
#	define loss_equals_delta(expected, actual, delta) _equals(__FILE__, __LINE__, expected, actual, false, delta)

#	define loss_not_equals(expected, actual) _equals(__FILE__, __LINE__, expected, actual, true)
#	define loss_not_equalsDelta(expected, actual, delta) _equals(__FILE__, __LINE__, expected, actual, true, delta)
#endif
