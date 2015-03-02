#include "defines.hpp"
#if UNIT_TEST_PROG == true

#ifndef UNIT_TESTS_HPP_INCLUDED
#define UNIT_TESTS_HPP_INCLUDED

#include <unittest++/UnitTest++.h>

#include "ansi_tests/cursor_move/test.hpp"

namespace
{
    void run_tests();
    
    
    inline void run_tests()
    {
        UnitTest::RunAllTests();
    }
    
    
}

#endif
#endif