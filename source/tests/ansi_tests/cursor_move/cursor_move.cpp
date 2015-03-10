#include "build_defines.hpp"
#if UNIT_TEST_PROG == true

#include <iostream>
#include <string>
#include <chrono>
#include <unistd.h>

#include "utility/ansi.hpp"
#include "cursor_move.hpp"

/*attn this set of functions is from ansi.cpp, so when you test, make sure this
 * is correct. */
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
    
    
    inline __attribute__((unused)) std::string csi_char()
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
    
    inline __attribute__((unused)) std::string to_string(const unsigned int& num)
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
    void display_in_direction(const std::string&, const ansi::cursor::direction&);
    void display_text_as_box(const std::string&, const unsigned int&, const unsigned int&);
    ansi::cursor::direction inverse_direction(const ansi::cursor::direction&);
    void display_box(const char&, const unsigned int&, const unsigned int&);
    void display_br(const std::string&);
    void msleep(const unsigned int&);
    
    
    
    inline void display_in_direction(const std::string& s, const ansi::cursor::direction& d)
    {
        using std::cout;
        using ansi::cursor::move;
        using ansi::display_ansi;
        
        if(d == ansi::cursor::right)
        {
            cout<< s;
        }
        else
        {
            for(std::string::const_iterator it(s.begin()); it != s.end(); ++it)
            {
                cout<< *it;
                if((d == ansi::cursor::up) || (d == ansi::cursor::down))
                {
                    ansi::display_ansi(move(1, d) + move(1, ansi::cursor::left));
                }
                else
                {
                    display_ansi(move(2, ansi::cursor::left));
                }
            }
        }
        cout.flush();
    }
    
    inline void display_text_as_box(const std::string& s, const unsigned int& width, const unsigned int& hieght)
    {
        using ansi::display_ansi;
        using ansi::cursor::move;
        
        ansi::cursor::direction dir_order[4] = {
                ansi::cursor::right,
                ansi::cursor::down,
                ansi::cursor::left,
                ansi::cursor::up};
        bool use_width(true);
        unsigned int side_length(width);
        
        for(unsigned int x(0), side(0); x < s.size();
                use_width = (((side + 1) % 2) == 0),
                side_length = ((use_width ? width : hieght) - 1),
                x += (((x + side_length) < s.size()) ? side_length : (s.size() - x)), 
                ++side)
        {
            display_in_direction(s.substr(x, 
                    (((x + side_length) >= s.size()) ? (s.size() - x) : side_length)), 
                    dir_order[side % 4]);
            display_ansi(move(1, inverse_direction(dir_order[(side % 4)])));
            display_ansi(move(1, dir_order[((side + 1) % 4)]));
        }
    }
    
    inline void display_box(const char& ch, const unsigned int& w, const unsigned int& h)
    {
        using ansi::display_ansi;
        using ansi::cursor::move;
        
        display_in_direction(std::string(w, ch), ansi::cursor::right);
        display_ansi(move(1, ansi::cursor::down) + move(1, ansi::cursor::left));
        display_in_direction(std::string((h - 2), ch), ansi::cursor::down);
        display_in_direction(std::string(w, ch), ansi::cursor::left);
        display_ansi(move(1, ansi::cursor::right) + move(1, ansi::cursor::up));
        display_in_direction(std::string((h - 2), ch), ansi::cursor::up);
    }
    
    inline ansi::cursor::direction inverse_direction(const ansi::cursor::direction& d)
    {
        switch(d)
        {
            case ansi::cursor::down:
            {
                return ansi::cursor::up;
            }
            break;
            
            case ansi::cursor::up:
            {
                return ansi::cursor::down;
            }
            break;
            
            case ansi::cursor::left:
            {
                return ansi::cursor::right;
            }
            break;
            
            case ansi::cursor::right:
            {
                return ansi::cursor::left;
            }
            break;
            
            default:
            {
            }
            break;
        }
        return (ansi::cursor::direction)0;
    }
    
    /* Display_bottom_right */
    inline void display_br(const std::string& s)
    {
        using namespace ansi;
        using cursor::jump;
        using std::cout;
        
        display_ansi(ansi::cursor::save_pos());
        
        unsigned int x(80), y(25);
        
        if(s.size() >= 80)
        {
            x = (s.size() % 80);
            if(x == 0) x = 1;
            y -= (s.size() / 80);
        }
        else
        {
            x -= s.size();
        }
        display_ansi(jump(x, y));
        display_ansi(ansi::display::clear_line(2) + ansi::display::clear_screen(0));
        cout<< s;
        cout.flush();
        display_ansi(ansi::cursor::restore_pos());
    }
    
    inline void msleep(const unsigned int& ms)
    {
        using namespace std::chrono;
        
        usleep(duration_cast<microseconds>(milliseconds(ms)).count());
    }
    
    
}

namespace test
{
    void test_cursor_movement()
    {
        using std::cout;
        using std::endl;
        using ansi::cursor::move;
        using ansi::display_ansi;
        
        std::string temps("Hello World!");
        
        display_br("Testing cursor movement...");
        cout<< "temps: \""<< temps<< "\""<< endl;
        cout<< "as a box: ";
        cout.flush();
        for(unsigned int x = 2; x < 5; ++x)
        {
            display_text_as_box(temps, x, x);
            display_ansi(move((x + 1), ansi::cursor::down));
        }
        display_ansi(ansi::cursor::jump(2, 1));
        display_box('*', 70, 20);
        display_ansi(move(21, ansi::cursor::down));
        display_br("DONE!");
        msleep(2000);
    }
    
    bool test_to_string(const int& x)
    {
        return (std::to_string(x) == ::to_string(x));
    }
    
    void test_clear_screen()
    {
        using std::cout;
        using std::endl;
        using namespace ansi;
        using ansi::display::clear_screen;
        
        std::string temps("Hello \nWorld!\nnewline??");
        
        for(unsigned int x = 0; x < 10; ++x)
        {
            display_ansi(ansi::cursor::hide(((x % 2) == 0)));
            if((x % 2) == 0)
            {
                display_ansi(clear_screen(2));
                display_ansi(ansi::cursor::jump(1, 1));
            }
            else
            {
                cout<< temps;
                cout.flush();
            }
            display_br(std::string("Testing clear screen: ") + 
                    (((x % 2) == 0) ? "hiding" : "showing"));
            msleep(500);
        }
    }
    
    
}

#endif