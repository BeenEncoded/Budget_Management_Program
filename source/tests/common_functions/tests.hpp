#include "build_defines.hpp"
#if UNIT_TEST_PROG == true
#ifndef TESTS_COMMON_FUNCTIONS_TESTS_HPP_INCLUDED
#define TESTS_COMMON_FUNCTIONS_TESTS_HPP_INCLUDED
#include <unittest++/UnitTest++.h>

#include "common_function_tests.hpp"

SUITE(common_function_tests_test_suite)
{
    /*this is purely in case of other key code sets:
    TEST(test_key_code_tests_test_case)
    {
        test::test_key_codes();
    }*/
    
    TEST(test_get_user_string_test_case)
    {
        CHECK(test::test_get_user_string());
    }
    
    
}

#endif
#endif