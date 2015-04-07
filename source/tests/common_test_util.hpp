#include "build_defines.hpp"
#if UNIT_TEST_PROG == true

#ifndef TESTS_COMMON_TEST_UTIL_HPP_INCLUDED
#define TESTS_COMMON_TEST_UTIL_HPP_INCLUDED

#include <sstream>
#include <string>

namespace test
{
    namespace common
    {
        namespace
        {
            template<typename type>
            type read_from_stream(const type& t)
            {
                type new_val;
                std::stringstream ss;
                
                ss<< t;
                ss>> new_val;
                return new_val;
            }
            
            
        }
        
        char random_char();
        char random_char(const std::string&);
        unsigned int random_number(const unsigned int&, const unsigned int&);
        std::string random_string(const std::string&, const unsigned int&, const unsigned int&);
        std::string random_string(const unsigned int&, const unsigned int&);
        
    }
}

#endif
#endif