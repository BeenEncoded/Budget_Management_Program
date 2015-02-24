#include <iostream>
#include <string>

#include "utility/ansi.hpp"
#include "cursor_move.hpp"

namespace
{
    void display_ansi(const std::string&);
    void display_in_direction(const std::string&, const ansi::direction&);
    void display_text_as_box(const std::string&, const unsigned int&, const unsigned int&);
    
    
    inline void display_ansi(const std::string& s)
    {
        using std::cout;
        
        for(std::string::const_iterator it(s.begin()); it != s.end(); ++it)
        {
            cout<< *it;
            cout.flush();
        }
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
        cout.flush();
    }
    
    
}