#include <string>

#include "global/global_defines.hpp"
#include "common.hpp"
#include "iofunctions.hpp"

namespace
{
    unsigned long long pow(const unsigned int&, const unsigned int&);
    
    
    /**
     * @brief raises a number to the specified power.
     * @param a number to apply exponent to.
     * @param b exponent.
     * @return (a ^ b)
     */
    inline unsigned long long pow(const unsigned int& a, const unsigned int& b)
    {
        if(b == 1) return a;
        else if(b == 0) return 1;
        
        unsigned long long newa(a);
        for(unsigned int x = 1; x < b; ++x) newa *= a;
        return newa;
    }
    
    
}

namespace common
{
    /**
     * @brief returns a specified digit within a number.
     * @param num the number to perform the operation on.
     * @param place the place you want to get.  For example,
     * if you want the digit at the 10's place, then you
     * would pass 2.
     * @return A single-digit number that represents the
     * digit at the specified place.
     */
    int digit(const unsigned int& num, const unsigned short& place)
    {
        return (((num % pow(10, place)) - (num % pow(10, (place - 1)))) / (10 ^ (place - 1)));
    }
    
    /**
     * @brief Retrieves the number of digits in a number.
     * @param num the integer to operate on
     * @return the number of digits in num.
     */
    int digit_count(const unsigned int& num)
    {
        int count(0);
        unsigned int temp(num);
        while(temp >= 10)
        {
            temp /= 10;
            ++count;
        }
        return ++count;
    }
    
    bool is_letter(const char& ch)
    {
        for(unsigned char x('a'), y('A'); ((x <= 'z') && (y <= 'Z')); ++x, ++y)
        {
            if((ch == (signed)x) || (ch == (signed)y))
            {
                return true;
            }
        }
        return false;
    }
    
    bool is_special(const char& ch)
    {
        return (std::string(SPECIALS).find(ch) != std::string::npos);
    }
    
    bool is_number(const char& ch)
    {
        return (std::string(NUMBERS).find(ch) != std::string::npos);
    }
    
    bool is_char(const char& ch)
    {
        return (is_letter(ch) || is_number(ch) || is_special(ch));
    }
    
    /**
     * @brief gets a user's key press.
     * @return The key a user has pressed.
     */
    key_code::key_code_data getch_funct()
    {
        using key_code::key_code_data;
        using key_code::is_listed_control;
        using key_code::might_be_control;
        
        key_code_data key;
        unsigned int x(0);
        
        do
        {
            key.control_d.push_back(input::getch().control_d[0]);
        }while(might_be_control(key) && !is_listed_control(key) && input::kbhit() && (x++ < 10));
        key.is_control = (key.control_d.size() > 1);
        return key;
    }
    
    char gkey()
    {
        input::cl();
        return input::getch().control_d[0];
    }
    
    key_code::key_code_data gkey_funct()
    {
        input::cl();
        return getch_funct();
    }
    
    
}