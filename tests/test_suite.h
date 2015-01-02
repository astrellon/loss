#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <exception>

#include <loss/return_codes.h>

#define run_suite(s)	\
	{	\
		s suite;	\
		suite.run();	\
		suite_count++;	\
	}
	
#define run_case(f)	\
	before_case(); \
	try { \
	if (!f()) { \
		_tests_failed++; \
	}}	catch (...)	{ \
		_tests_failed++; \
		on_error();	\
	}\
	_total_tests++;	\
	after_case();

namespace loss
{
    namespace tests
    {
        class TestSuite
        {
            public:
                TestSuite();
                virtual ~TestSuite();

                virtual void before_suite();
                virtual void after_suite();

                virtual void before_case();
                virtual void after_case();

                virtual void on_error();

                virtual void run();
                virtual void run_cases() = 0;

                virtual std::string suite_name() const;

                static void failed_test(const std::string &name);
                static const std::vector<std::string> &failed_tests();

            protected:
                uint32_t _tests_failed;
                uint32_t _total_tests;

                static std::vector<std::string> s_failed_tests;
                
                static void disp_error_line(const char *file, uint32_t line);
                static void _assert(bool a, const char *file, uint32_t line);
                static void _assert(uint32_t a, const char *file, uint32_t line);
                static void _assert(void *a, const char *file, uint32_t line);

                template <class T>
                    static void disp_error(T expected, T actual, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "\n- Expected: >" << expected << "<\n- Actual: >" << actual << "<\n";
                    }

                template <class T>
                    static void disp_not_error(T expected, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "- Did not expect: " << expected << "\n";
                    }

                template <class T>
                    static void disp_error(T expected, T actual, double delta, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        std::cout << "- Expected: " << expected << "\n- Actual  : " << actual << "\n- Delta   : " << delta << "\n";
                    }

                template <class T>
                    static void disp_not_error(T expected, double delta, const char *file, uint32_t line) 
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
                    static void disp_array_error(const T *expected, const T *actual, int n, int errn, double delta, const char *file, uint32_t line)
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
                    static void disp_array_not_error(const T *expected, int n, int errn, double delta, const char *file, uint32_t line)
                    {
                        disp_error_line(file, line);
                        std::stringstream ss;
                        ss << "- Did not expect: ";
                        concat_array(expected, n, ss);
                        ss << "\n- Delta: " << delta
                            << "\n- Index: " << errn;
                        std::cout << ss;
                    }

                template <class T>
                    static void _simple_compare(T expected, T actual, bool not_compare, const char *file, uint32_t line)
                    {
                        if (not_compare)
                        {	
                            if (expected == actual)
                            { 
                                disp_not_error(expected, file, line); 
                                throw std::runtime_error(""); 
                            }	
                        } 
                        else 
                        {	
                            if (expected != actual)
                            { 
                                disp_error(expected, actual, file, line); 
                                throw std::runtime_error(""); 
                            }	
                        }
                    }

                static void _equals(const char *file, uint32_t line, int32_t expected, int32_t actual, bool not_compare);
                static void _equals(const char *file, uint32_t line, loss::ReturnCode expected, loss::ReturnCode actual, bool not_compare);
                static void _equals(const char *file, uint32_t line, uint32_t expected, uint32_t actual, bool not_compare);
                static void _equals(const char *file, uint32_t line, double expected, double actual, bool not_compare);
                static void _equals(const char *file, uint32_t line, const char *expected, const char *actual, bool not_compare);
                static void _equals(const char *file, uint32_t line, double expected, double actual, bool not_compare, double delta=0.00001);

                static void _equals_str(const char *file, uint32_t line, const char *expected, const char *actual, bool not_compare);
                static void _equals_str(const char *file, uint32_t line, const char *expected, const std::string &actual, bool not_compare);

                template <class T>
                    static void _equals_array(const char *file, uint32_t line, const T *expected, const T *actual, bool not_compare, double delta=0.00001, int n=16)
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
                            throw std::runtime_error("");
                        }
                        else if (!not_compare && !equal) 
                        {
                            disp_array_error(expected, actual, n, i, delta, file, line);
                            throw std::runtime_error("");
                        }
                    }
        };

    }
}

#define loss_assert(a) \
    _assert(a, __FILE__, __LINE__)

//  General equals
#define loss_equals(expected, actual) \
    _equals(__FILE__, __LINE__, expected, actual, false)
#define loss_equals_delta(expected, actual, delta) \
    _equals(__FILE__, __LINE__, expected, actual, false, delta)

//  General not equals
#define loss_not_equals(expected, actual) \
    _equals(__FILE__, __LINE__, expected, actual, true)
#define loss_not_equalsDelta(expected, actual, delta) \
    _equals(__FILE__, __LINE__, expected, actual, true, delta)

//  String equals
#define loss_equals_str(expected, actual)	\
    _equals_str(__FILE__, __LINE__, expected, actual, false)
#define not_equalsStr(expected, actual)	\
    _equals_str(__FILE__, __LINE__, expected, actual, true)

//  _array equals
#define loss_equals_array(expected, actual, n)  \
    _equals_array(__FILE__, __LINE__, expected, actual, false, 0.00001, n)
#define loss_not_equals_array(expected, actual, n)  \
    _equals_array(__FILE__, __LINE__, expected, actual, true, 0.00001, n)
