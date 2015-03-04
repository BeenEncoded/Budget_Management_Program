#include <vector>
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>

#include "utility/ansi.hpp"

namespace
{
    template<unsigned char type>
    struct const_char_type
    {
        static const unsigned char value;
    };
    
    template<unsigned char type> const unsigned char const_char_type<type>::value = type;
    
    //some misc characters:
    typedef const_char_type<0x1b> esc;
    
    std::string csi_char();
    int digit(const unsigned int&, const unsigned short&);
    int digit_count(const unsigned int&);
    unsigned long long pow(const unsigned int&, const unsigned int&);
    __attribute__((unused)) std::string to_string(const int&);
    std::string to_string(const unsigned int&);
    
    
    inline std::string csi_char()
    {
        return std::string({esc::value, '['});
    }
    
    inline int digit(const unsigned int& num, const unsigned short& place)
    {
        return (((num % pow(10, place)) - (num % pow(10, (place - 1)))) / pow(10, (place - 1)));
    }
    
    inline int digit_count(const unsigned int& num)
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
    
    inline unsigned long long pow(const unsigned int& num, const unsigned int& exp)
    {
        if(exp == 0) return 1;
        else if(exp == 1) return num;
        
        unsigned long long newnum(num);
        for(unsigned int x = 1; x < exp; ++x) newnum *= num;
        return newnum;
    }
    
    /** I implemented my own to_string function so that this code could be
     * C++98 compatible. */
    __attribute__((unused)) inline std::string to_string(const int& num)
    {
        unsigned int tempnum(((num < 0) ? (num * (-1)) : num)), digits(digit_count(tempnum));
        std::string number;
        
        char numbers[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for(unsigned int x = 0; x < digits; ++x) number.insert(number.begin(), numbers[digit(tempnum, (x + 1))]);
        if(num < 0) number.insert(number.begin(), '-');
        return number;
    }
    
    inline std::string to_string(const unsigned int& num)
    {
        unsigned int digits(digit_count(num));
        std::string number;
        
        char numbers[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for(unsigned int x = 0; x < digits; ++x) number.insert(number.begin(), numbers[digit(num, (x + 1))]);
        return number;
    }
    
    
}

namespace ansi
{
    void display_ansi(const std::string& s)
    {
        using std::cout;
        
        for(std::string::const_iterator it(s.begin()); it != s.end(); ++it)
        {
            cout<< *it;
        }
        cout.flush();
    }
    
    
    namespace cursor
    {
        /* Moves the cursor "count" times in a specified direction. */
        std::string move(const unsigned int& count, const direction& dir)
        {
            char d[4] = {'A', 'B', 'D', 'C'};
            std::string code(csi_char() + to_string(count) + d[dir]);
            return code;
        }
        
        std::string save_pos()
        {
            return (csi_char() + 's');
        }
        
        std::string restore_pos()
        {
            return (csi_char() + 'u');
        }
        
        /* Jumps the cursor to the specified X position.  1 is the beginning of the
         * line. */
        std::string goto_x(const unsigned int& x)
        {
            return (csi_char() + ::to_string(x) + 'G');
        }
        
        /* jumps the cursor to a set of coordinates.  (1, 1) is the top-left corner. */
        std::string jump(const unsigned int& x, const unsigned int& y)
        {
            return (csi_char() + ::to_string(y) + ';' + ::to_string(x) + 'H');
        }
        
        std::string hide(const bool& hidden)
        {
            if(hidden) return (csi_char() + "?25l");
            return (csi_char() + "?25h");
        }
        
        
    }
    
    namespace display
    {
        std::string clear_screen(const unsigned short& arg)
        {
            return (csi_char() + ::to_string(arg) + 'J');
        }
        
        std::string clear_line(const unsigned short& arg)
        {
            return (csi_char() + ::to_string(arg) + 'K');
        }
        
        
    }
    
    
}