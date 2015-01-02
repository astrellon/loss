#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <exception>

#include <loss/return_codes.h>

#define run_case(f)	\
	before_case(); \
	try { \
        f(); \
	}	catch (...)	{ \
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
                TestSuite(std::ostream &output);
                virtual ~TestSuite();

                virtual void before_suite();
                virtual void after_suite();

                virtual void before_case();
                virtual void after_case();

                virtual void on_error();

                virtual bool run();
                virtual void run_cases() = 0;

                virtual std::string suite_name() const;

                void failed_test(const std::string &name);

            protected:
                uint32_t _tests_failed;
                uint32_t _total_tests;
                std::ostream &_out;

                void disp_error_line(const char *file, uint32_t line);
                void _assert(bool a, const char *file, uint32_t line);
                void _assert(uint32_t a, const char *file, uint32_t line);
                void _assert(void *a, const char *file, uint32_t line);

                template <class T>
                    void disp_error(T expected, T actual, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        _out << "\n- Expected: >" << expected << "<\n- Actual: >" << actual << "<\n";
                    }

                template <class T>
                    void disp_not_error(T expected, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        _out << "- Did not expect: " << expected << "\n";
                    }

                template <class T>
                    void disp_error(T expected, T actual, double delta, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        _out << "- Expected: " << expected << "\n- Actual  : " << actual << "\n- Delta   : " << delta << "\n";
                    }

                template <class T>
                    void disp_not_error(T expected, double delta, const char *file, uint32_t line) 
                    {
                        disp_error_line(file, line);
                        _out << "- Did not expect: " << expected << "\n- Delta   : " << delta << "\n";
                    }

                template <class T>
                    void concat_array(const T *array, int n, std::stringstream &ss)
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
                    void disp_array_error(const T *expected, const T *actual, int n, int errn, double delta, const char *file, uint32_t line)
                    {
                        disp_error_line(file, line);
                        _out <<   "- Expected: ";
                        concat_array(expected, n, _out);
                        _out << "\n- Actual  : ";
                        concat_array(actual, n, _out);
                        _out << "\n- Delta   : " << delta
                            << "\n- Index   : " << errn;
                    }
                template <class T>
                    void disp_array_not_error(const T *expected, int n, int errn, double delta, const char *file, uint32_t line)
                    {
                        disp_error_line(file, line);
                        _out << "- Did not expect: ";
                        concat_array(expected, n, _out);
                        _out << "\n- Delta: " << delta
                            << "\n- Index: " << errn;
                    }

                template <class T>
                    void _simple_compare(T expected, T actual, bool not_compare, const char *file, uint32_t line)
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

                void _equals(const char *file, uint32_t line, int32_t expected, int32_t actual, bool not_compare);
                void _equals(const char *file, uint32_t line, loss::ReturnCode expected, loss::ReturnCode actual, bool not_compare);
                void _equals(const char *file, uint32_t line, uint32_t expected, uint32_t actual, bool not_compare);
                void _equals(const char *file, uint32_t line, double expected, double actual, bool not_compare);
                void _equals(const char *file, uint32_t line, const char *expected, const char *actual, bool not_compare);
                void _equals(const char *file, uint32_t line, double expected, double actual, bool not_compare, double delta=0.00001);

                void _equals_str(const char *file, uint32_t line, const char *expected, const char *actual, bool not_compare);
                void _equals_str(const char *file, uint32_t line, const char *expected, const std::string &actual, bool not_compare);

                template <class T>
                    void _equals_array(const char *file, uint32_t line, const T *expected, const T *actual, bool not_compare, double delta=0.00001, int n=16)
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

        class Suites
        {
            public:
                Suites(std::ostream &output);

                template <class T>
                    void run_suite()
                    {
                        T suite(_out);
                        if (!suite.run())
                        {
                            _failed_tests.push_back(suite.suite_name());
                        }
                        else
                        {
                            _passed_tests.push_back(suite.suite_name());
                        }
                    }
                
                const std::vector<std::string> &passed_tests() const;
                const std::vector<std::string> &failed_tests() const;

            private:
                std::vector<std::string> _passed_tests;
                std::vector<std::string> _failed_tests;
                std::ostream &_out;
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
