#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <iostream>
#include <string>
#include <vector>

#include "global/global_defines.hpp"
#include "utility/time_class.hpp"

namespace common
{
    template<typename type> struct result_data;
    
    /**
     * @class result_data
     * @author Jonathan Whitlock
     * @date 04/12/2015
     * @file common.hpp
     * @brief Represents the result of an operation.  Without template
     * arguments, the default type is void*.  If given no template arguments,
     * it is assumed that the member variable 'data' is not used.
     */
    template<typename type = void*>
    struct result_data
    {
        explicit result_data() noexcept;
        result_data(result_data<type>&&) noexcept;
        result_data(const result_data<type>&) noexcept;
        
        ~result_data();
        
        result_data<type>& operator=(const result_data<type>&);
        result_data<type>& operator=(result_data<type>&&) noexcept;
        bool operator!() const;
        bool operator==(const result_data<type>&) const;
        bool operator!=(const result_data<type>&) const;
        
        bool success;
        type data;
        std::string message;
    };
    
    
    int digit(const unsigned int&, const unsigned short&);
    int digit_count(const unsigned int&);
    bool is_letter(const char&);
    bool is_special(const char&);
    bool is_number(const char&);
    bool is_char(const char&);
    bool get_user_str(std::string&);
    void wait();
    void cls();
    void center(const std::string&);
    bool prompt_user(const std::string&);
    std::string peek_string(std::istream&, const unsigned int&);
    std::istream::pos_type inavail(std::istream&);
    std::string time_disp(const tdata::time_class&);
    std::string date_disp(const tdata::time_class&);
    template<typename type1, typename type2> void distribute_equally(const type1&, std::vector<type2>&, void (*access)(type2&, type1*&));
    template<typename type> bool load_from_file(const std::string&, type&);
    template<typename type> bool save_to_file(const std::string&, const type&);
    
}

#endif