#include "build_defines.hpp"
#if UNIT_TEST_PROG == true
#ifndef TESTS_MONETARY_ALLOCATION_TESTS_STREAM_OPS_STREAM_OPS_TEST_HPP_INCLUDED
#define TESTS_MONETARY_ALLOCATION_TESTS_STREAM_OPS_STREAM_OPS_TEST_HPP_INCLUDED

#include <unittest++/UnitTest++.h>

#include "stream_ops.hpp"

SUITE(budget_data_stream_operator_tests_test_suite)
{
    TEST(test_budget_data_streams_test_case)
    {
        using test::test_budget_data_streams;
        
        for(unsigned int x(0); x < 10000; ++x) CHECK(test_budget_data_streams());
    }
    
    TEST(test_budget_data_multiple_writes_test)
    {
        using test::test_multiple_budget_streams;
        
        for(unsigned int x(0); x < 10000; ++x) CHECK(test_multiple_budget_streams());
    }
    
    TEST(test_budget_data_streams_no_budgets_test_case) //an empty vector
    {
        CHECK(test::test_empty_vec());
    }
    
    
}

#endif
#endif