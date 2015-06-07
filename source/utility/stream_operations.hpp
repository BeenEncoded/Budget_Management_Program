#ifndef UTILITY_STREAM_OPERATIONS_HPP_INCLUDED
#define UTILITY_STREAM_OPERATIONS_HPP_INCLUDED
#include <string>
#include <iostream>
#include <vector>

namespace utility
{
    std::string peek_string(std::istream&, const unsigned int&);
    
    template<class type> bool safe_getline(std::istream&, type&, const char&);
    template<> bool safe_getline<std::string>(std::istream&, std::string&, const char&);
    template<typename type> std::istream& in_mem(std::istream&, type&);
    template<typename type> std::ostream& out_mem(std::ostream&, const type&);
    template<typename type> std::ostream& write_vector(std::ostream&, const std::vector<type>&);
    template<typename type> std::istream& read_vector(std::istream&, std::vector<type>&);
    
}

#endif