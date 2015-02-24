#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>

#include "utility/ansi.hpp"
#include "cursor_move.hpp"


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


namespace
{
    void display_ansi(const std::string&);
    void display_in_direction(const std::string&, const ansi::direction&);
    void display_text_as_box(const std::string&, const unsigned int&, const unsigned int&);
    
    
    inline void display_ansi(const std::string& s) //todo make part of source
    {
        using namespace std::chrono;
        using std::cout;
        
        for(std::string::const_iterator it(s.begin()); it != s.end(); ++it)
        {
            cout<< *it;
        }
        cout.flush();
        usleep(duration_cast<microseconds>(milliseconds(33)).count());
    }
    
    inline void display_in_direction(const std::string& s, const ansi::direction& d)
    {
        using std::cout;
        
        if(d == ansi::right) cout<< s;
        else
        {
            for(std::string::const_iterator it(s.begin()); it != s.end(); ++it)
            {
                cout<< *it;
                if((d == ansi::up) || (d == ansi::down)) display_ansi(ansi::move(1, d) + ansi::move(1, ansi::left));
                else display_ansi(ansi::move(2, ansi::left));
            }
        }
        cout.flush();
    }
    
    inline void display_text_as_box(const std::string& s, const unsigned int& width, const unsigned int& hieght)
    {
        ansi::direction dir_order[4] = {ansi::right, ansi::down, ansi::left, ansi::up};
        bool use_width(true);
        
        for(unsigned int x(0), side(0); x < s.size(); use_width = ((side % 2) == 0), x += (use_width ? width : hieght), ++side)
        {
            display_in_direction(s.substr(x, 
                    (((x + (use_width ? width : hieght)) >= s.size()) ? (s.size() - x) : (use_width ? width : hieght))), 
                    dir_order[side % 4]);
        }
    }
    
    
}

namespace test
{
    void test_cursor_movement()
    {
        using std::cout;
        using std::endl;
        
        std::string temps("Hello World!");
        
        cout<< "temps: \""<< temps<< "\""<< endl;
        cout<< "as a box: ";
        display_text_as_box(temps, 3, 3);
        display_ansi(ansi::move(4, ansi::down));
    }
    
    bool test_to_string(const int& x)
    {
        using std::cout;
        using std::endl;
        
        bool tempb(std::to_string(x) == ::to_string(x));
        cout<< "Number: "<< x<< endl;
        cout<< "std: "<< std::to_string(x)<< endl;
        cout<< "manual: "<< ::to_string(x)<< endl;
        return tempb;
    }
    
    
}