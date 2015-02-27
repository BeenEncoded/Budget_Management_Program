#ifndef ANSI_HPP_INCLUDED
#define ANSI_HPP_INCLUDED

namespace ansi
{
    enum direction
    {
        up = 0,
        down,
        left,
        right
    };
    
    
    std::string move(const unsigned int&, const direction&);
    void display_ansi(const std::string&);
    
}

#endif