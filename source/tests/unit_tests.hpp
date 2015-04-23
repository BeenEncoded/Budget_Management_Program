#if UNIT_TEST_PROG == true

#ifndef UNIT_TESTS_HPP_INCLUDED
#define UNIT_TESTS_HPP_INCLUDED

#include <unittest++/UnitTest++.h>
#include <ctime>
#include <cstdlib>

//Unit Tests:

#include "ansi_tests/tests_included.hpp"
#include "monetary_allocation_tests/tests_included.hpp"
//#include "common_functions/tests.hpp"

namespace
{
    void run_tests();
    
    
    inline void run_tests()
    {
        srand(std::time(nullptr));
        UnitTest::RunAllTests();
    }
    
    
}

#endif
#endif