#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

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
    
    template<typename type>
    std::istream& in_mem(std::istream& in, type& t)
    {
        t = type();
        char *mem(new char[sizeof(type)]);
        
        in.peek();
        if(in.good())
        {
            for(unsigned int x = 0; ((x < sizeof(type)) && in.good()); ++x)
            {
                mem[x] = in.get();
            }
            memcpy(&t, mem, sizeof(type));
        }
        delete[] mem;
        return in;
    }
    
    template<typename type>
    std::ostream& out_mem(std::ostream& out, const type& t)
    {
        char *mem(new char[sizeof(type)]);
        
        memcpy(mem, &t, sizeof(type));
        if(out.good())
        {
            for(unsigned int x = 0; x < sizeof(type); ++x)
            {
                out<< mem[x];
            }
        }
        delete[] mem;
        return out;
    }
    
    /**
     * @brief distributes an integer equally throughout a list of elements
     * given the element type can be accessed.  Distribution is done by
     * settings each element to (val / v.size()).  The remainder is then added,
     * if there is any.
     * @param val value to distribute.
     * @param v list of elements.
     * @param access The function used to access the number stored at
     * element.  Provided for use with structs, etc...
     */
    template<typename type1, typename type2>
    inline void distribute_equally(const type1& val, std::vector<type2> v,
            void (*access)(type2&, type1*&))
    {
        type1 *value(nullptr);
        for(typename std::vector<type2>::iterator it(v.begin()); it != v.end(); ++it)
        {
            access(*it, value);
            (*value) = ((type1)val / (type1)v.size());
        }
        for(unsigned int x = 0; x < ((type1)val % (type1)v.size()); ++x)
        {
            access(v[x], value);
            ++(*value);
        }
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