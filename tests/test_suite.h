#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <exception>

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
                int _tests_failed;
                int _total_tests;

                static std::vector<std::string> s_failed_tests;
        };

    }
}
