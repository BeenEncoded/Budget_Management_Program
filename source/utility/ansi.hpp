#ifndef ANSI_HPP_INCLUDED
#define ANSI_HPP_INCLUDED
#include <string>

namespace ansi
{
    void display_ansi(const std::string&);
    
    //functions that modify the cursor and it's location:
    namespace cursor
    {
        enum direction
        {
            up = 0,
            down,
            left,
            right
        };
        
        
        std::string move(const unsigned int&, const direction&);
        std::string save_pos();
        std::string restore_pos();
        std::string goto_x(const unsigned int&);
        std::string hide(const bool&);
        std::string jump(const unsigned int&, const unsigned int&);
        
    }
    
    //functions that modify the way glyphs are displayed, or the glyphs cuyrrently being displayed:
    namespace display
    {
        std::string clear_screen(const unsigned short&); //0: erase to end; 1: erase to beg; 2: erase all
        std::string clear_line(const unsigned short&);   //(doesn't change cursor pos) 0: erase to end; 1: erase to beg; 2: erase all
        
    }
    
    
}

#endif