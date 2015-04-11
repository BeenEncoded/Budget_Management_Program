#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <iostream>
#include <string>
#include <vector>

namespace common
{
    int digit(const unsigned int&, const unsigned short&);
    int digit_count(const unsigned int&);
    bool is_letter(const char&);
    bool is_special(const char&);
    bool is_number(const char&);
    bool is_char(const char&);
    key_code::key_code_data getch_funct();
    char gkey();
    key_code::key_code_data gkey_funct();
    bool get_user_str(std::string&);
    void wait();
    void cls();
    void cl();
    void center(const std::string&);
    bool prompt_user(const std::string&);
    template<class type> bool safe_getline(std::istream&, type&, const char&);
    template<> bool safe_getline<std::string>(std::istream&, std::string&, const char&);
    template<typename type> std::istream& in_mem(std::istream&, type&);
    template<typename type> std::ostream& out_mem(std::ostream&, const type&);
    template<typename type1, typename type2> void distribute_equally(const type1&, std::vector<type2>&, void (*access)(type2&, type1*&));
    template<typename type> bool load_from_file(const std::string&, type&);
    template<typename type> bool save_to_file(const std::string&, const type&);
    
}

#endif