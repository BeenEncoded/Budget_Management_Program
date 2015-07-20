#ifdef UNIT_TEST_PROG
#ifndef TESTS_TIMEFRAME_TESTS_OVERLAP_TESTS_TEST_HPP_INCLUDED
#define TESTS_TIMEFRAME_TESTS_OVERLAP_TESTS_TEST_HPP_INCLUDED
#include <unittest++/UnitTest++.h>

#include "overlap_test.hpp"

SUITE(overlap_tests_test_suite)
{
    TEST(timeframe_overlap_testing_test_case)
    {
        CHECK(testing::overlap_is_good());
    }
    
}

#endif
#endif