#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <iostream>
#include <string>
#include <sstream>

#include "global/global_defines.hpp"

namespace common
{
     /**
      * @brief Safely retrieves a line from a file given a specified delimiter.  
      * In case of failure, it will leave the stream in the state it was in
      * before the function call.  Returns true/false based on success.
      * @param in std::istream to read from
      * @param t object to read into
      * @param delimiter delimiter to read to
      * @return bool true if the read was a success, false otherwise.
      */
    template<class type>
    inline bool safe_getline(std::istream& in, type& t, const char& delimiter)
    {
        std::string temps;
        bool success(false);
        std::istream::pos_type previous_position(in.tellg());
        
        t = type();
        
        if(in.good())
        {
            std::getline(in, temps, delimiter);
            if(in.good())
            {
                std::stringstream ss;
                ss<< temps;
                ss>> t;
                success = true;
            }
            else if(!in.eof() && in.fail())
            {
                /* So, here we are: we didn't reach the end of the file, 
                 but somwhere there was a big mistake... the program will
                 now attempt to salvage the situation: */
                in.seekg(previous_position);
                success = false;
            }
        }
        return success;
    }

    /**
     * @brief A specialization of safe_getline for strings.
     * @param in std::istream to read from
     * @param s the string to read into
     * @param delimiter the delimiter to read up to.
     * @return a bool: true if the read was a success, false otherwise.
     */
    template<>
    inline bool safe_getline<std::string>(std::istream& in, std::string& s, const char& delimiter)
    {
        bool success(false);
        std::istream::pos_type previous_position(in.tellg());
        
        s.erase();
        
        if(in.good())
        {
            std::getline(in, s, delimiter);
            if(in.good()) success = true;
            else if(!in.eof() && in.fail())
            {
                /* So, here we are: we didn't reach the end of the file, 
                 but somwhere there was a big mistake... the program will
                 now attempt to salvage the situation: */
                in.seekg(0, std::ios::beg);
                in.seekg(previous_position);
                success = false;
            }
        }
        return success;
    }
    
    int digit(const unsigned int&, const unsigned short&);
    int digit_count(const unsigned int&);
    bool is_letter(const char&);
    bool is_special(const char&);
    bool is_number(const char&);
    bool is_char(const char&);
    key_code::key_code_data getch_funct();
    char gkey();
    key_code::key_code_data gkey_funct();
    
}

#endif