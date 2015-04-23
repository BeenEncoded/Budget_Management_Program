#if UNIT_TEST_PROG == true

#ifndef ANSI_CURSOR_MOVE_TESTS_HPP_INCLUDED
#define ANSI_CURSOR_MOVE_TESTS_HPP_INCLUDED
#include <unittest++/UnitTest++.h>
#include "cursor_move.hpp"

SUITE(cursor_move_test_suite)
{
    TEST(to_string_manual_test_case)
    {
        CHECK(test::test_to_string(3));
    }
    
    TEST(cursor_move_test_case)
    {
        test::test_cursor_movement();
    }
    
    TEST(clear_screen_test_test_case)
    {
        test::test_clear_screen();
    }
    
    
}

#endif
#endif