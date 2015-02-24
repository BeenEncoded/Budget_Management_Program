#if UNIT_TEST_PROG == true

#ifndef ANSI_CURSOR_MOVE_TESTS_HPP_INCLUDED
#define ANSI_CURSOR_MOVE_TESTS_HPP_INCLUDED
#include <unittest++/UnitTest++.h>
#include "cursor_move.hpp"

SUITE(cursor_move_test_suite)
{
    TEST(cursor_move_test_case)
    {
        test::test_cursor_movement();
    }
    
    
}

#endif
#endif