#include <string>
#include <vector>
#include <iostream>

#include "utility/ansi.hpp"
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
    
    void cl()
    {
        input::cl();
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
     * @brief gets a user's key press.  Does not clear the buffer.
     * @return The key a user has pressed.
     */
    key_code::key_code_data getch_funct()
    {
        using key_code::key_code_data;
        using key_code::is_listed_control;
        using key_code::might_be_control;
        
        key_code_data key;
        
        key.is_control = true;
        do
        {
            key.control_d.emplace_back(input::getch().control_d[0]);
        }while(might_be_control(key) && input::kbhit());
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
    
    /**
     * @brief Provides a more complete user input: pressing "escape" cancels the
     * input, and support for other features can be easily added.
     * @param inp the string to store input in.
     * @return A bool: true if the user didn't cancel.
     */
    bool get_user_str(std::string& inp)
    {
        using std::cout;
        using ansi::display_ansi;
        using ansi::cursor::move;
        using ansi::display::clear_line;
        using ansi::cursor::save_pos;
        using ansi::cursor::restore_pos;
        using key_code::key_code_data;
        
        unsigned int cursor_pos(0); //position of cursor in "inp"
        bool finished(false), canceled(false);
        key_code_data ch;
        
        inp.erase();
        display_ansi(save_pos());
        input::cl();
        do
        {
            //update display and cursor:
            display_ansi(restore_pos());
            display_ansi(clear_line(0));
            cout<< inp;
            cout.flush();
            display_ansi(restore_pos());
            if(cursor_pos > 0)
            {
                display_ansi(move(cursor_pos, ansi::cursor::direction::right));
            }
            
            //get user key:
            ch = common::getch_funct();
            if(key_code::is_listed_control(ch))
            {
                using namespace key_code::code;
                using key_code::keys;
                
                if(ch == keys[backspace::value])
                {
                    if(cursor_pos > 0)
                    {
                        inp.erase((inp.begin() + (--cursor_pos)));
                    }
                }
                else if(ch == keys[del::value])
                {
                    if(cursor_pos < inp.size())
                    {
                        inp.erase(inp.begin() + cursor_pos);
                    }
                }
                else if(ch == keys[left::value])
                {
                    if(cursor_pos > 0) --cursor_pos;
                }
                else if(ch == keys[right::value])
                {
                    if(cursor_pos < inp.size()) ++cursor_pos;
                }
                else if(ch == keys[end::value])
                {
                    cursor_pos = inp.size();
                }
                else if(ch == keys[home::value])
                {
                    cursor_pos = 0;
                }
                else if(ch == keys[escape::value])
                {
                    inp.erase();
                    canceled = true;
                    finished = true;
                }
            }
            else if(!ch.control_d.empty())
            {
                if(common::is_char(ch.control_d[0]))
                {
                    inp.insert((inp.begin() + cursor_pos), ch.control_d[0]);
                    if(cursor_pos < inp.size()) ++cursor_pos;
                }
                else
                {
                    if(ch.control_d[0] == '\n') finished = true;
                }
            }
        }while(!finished);
        return !canceled;
    }
    
    void wait()
    {
        for(unsigned int x(0); x < 2; ++x) std::cout<< std::endl;
        std::cout<< "Press anything to continue...";
        std::cout.flush();
        common::gkey();
    }
    
    void center(const std::string& s)
    {
        using std::cout;
        using std::endl;
        
        unsigned int center(((70 / 2) - (s.size() / 2)));
        
        if((s.size() / 2) >= (70 / 2))
        {
            center = 0;
        }
        if(center > 0) cout<< std::string(center, ' ');
        cout<< s<< endl;
    }
    
    void cls()
    {
        ansi::display_ansi(ansi::display::clear_screen(2) + ansi::cursor::jump(1, 1));
    }
    
    bool prompt_user(const std::string& message)
    {
        using std::cout;
        using std::endl;
        
        char ch;
        bool finished(false), result(false);
        
        do
        {
            cls();
            cout<< endl;
            center((message.empty() ? "Are you sure?" : message));
            cout<< endl<< endl;
            center("Yes or no?");
            cout.flush();
            
            ch = gkey();
            ch = std::tolower(ch);
            if(ch == 'y')
            {
                result = true;
                finished = true;
            }
            else if(ch == 'n')
            {
                finished = true;
            }
        }while(!finished);
        return result;
    }
    
    
}