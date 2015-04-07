#include "build_defines.hpp"
#if UNIT_TEST_PROG == true

#include <cstdlib>
#include <string>
#include "common/global/global_defines.hpp"

#include "common_test_util.hpp"

namespace
{
}

namespace test
{
    namespace common
    {
        /**
         * @brief Returns a random number between min and max (inclusive).
         * Does not check whether min < max.
         * @param min minimum bound
         * @param max maximum bound
         * @return a random number >= min && <= max.
         */
        unsigned int random_number(const unsigned int& min, const unsigned int& max)
        {
            return ((rand() % ((max + 1) - min)) + min);
        }
        
        /**
         * @return A random character.
         */
        char random_char()
        {
            return random_char((std::string(LETTERS) + NUMBERS + std::string(SPECIALS)));
        }
        
        /**
         * @brief Returns a random character out of a string.  Can be used
         * to limit the character set from which a random character is drawn.
         * @param chars The function will return a character referenced from a
         * random index of this string.
         * @return a random character.
         */
        char random_char(const std::string& chars)
        {
            return chars[random_number(0, (chars.size() - 1))];
        }
        
        /**
         * @brief Returns a random string.  The string's size is randomly generated, and
         * is anywhere between min and max (inclusive).
         * @param chars a string containing the characters that can be chosen.
         * @param min the minimum size.
         * @param max the maximum size.
         * @return a random string.
         */
        std::string random_string(const std::string& chars, const unsigned int& min, const unsigned int& max)
        {
            std::string temps;
            unsigned int size(random_number(min, max));
            
            for(unsigned int x(0); x < size; ++x) temps += random_char(chars);
            return temps;
        }
        
        /**
         * @brief Returns a random string.  The string's size is randomly generated, and
         * is anywhere between min and max (inclusive).
         * @param min the minimum size.
         * @param max the maximum size.
         * @return a random string.
         */
        std::string random_string(const unsigned int& min, const unsigned int& max)
        {
            return random_string((std::string(LETTERS) + NUMBERS + std::string(SPECIALS)), min, max);
        }
        
        
    }
}

#endif