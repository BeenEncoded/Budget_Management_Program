#ifdef UNIT_TEST_PROG
#ifndef TESTS_TIMEFRAME_TESTS_STREAM_OPERATORS_TEST_HPP_INCLUDED
#define TESTS_TIMEFRAME_TESTS_STREAM_OPERATORS_TEST_HPP_INCLUDED

#include <unittest++/UnitTest++.h>

#include "test_stream_ops.hpp"

SUITE(test_time_interval_type_stream_ops_test_suite)
{
    TEST(time_interval_type_test_case)
    {
        using testing::test_interval_streams;
        
        bool tempb{test_interval_streams()};
        CHECK(tempb);
        
        for(unsigned int x{0}; ((x < 1000) && tempb); ++x)
        {
            tempb = test_interval_streams();
            CHECK(tempb);
        }
    }
    
    
}

#endif
#endif