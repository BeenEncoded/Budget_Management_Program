#ifndef GLOBAL_DEFINES_HPP_INCLUDED
#define GLOBAL_DEFINES_HPP_INCLUDED
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

#define LETTERS "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"
#define NUMBERS "1234567890"
#define SPECIALS "`~!@#$%^&*()-_=+[{]}\\|;:\'\",<.>/? "
#define GSTRING_CANCEL "oiwydfp980  q30f8p90c9-i130894f=i-=23f=-    0w2g]["

//this should be used to report programming mistakes...  returning errors for things that should never happen.
#define ethrow(MSG) throw std::runtime_error(std::string(std::string(__FILE__) + \
"    Exception thrown at line " + std::to_string(__LINE__) + \
": " + std::string(MSG)))

template<char>
struct const_char_type
{
    static const char value;
};

template<unsigned int>
struct const_int_type
{
    static const unsigned int value;
};

template<char type> const char const_char_type<type>::value = type;
template<unsigned int type> const unsigned int const_int_type<type>::value = type;

typedef const_char_type<0x00> mem_delim;
typedef const_char_type<0x01> struct_delim;
typedef const_char_type<0x02> submem_delim;
typedef const_char_type<0x03> substruct_delim;

typedef const_int_type<40> h_center;
typedef const_int_type<8> v_center;


#endif
