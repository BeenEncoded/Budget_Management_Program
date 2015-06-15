#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>

#include "stream_operations.hpp"
#include "data/budget_data.hpp"

namespace
{
}

namespace utility
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
    bool safe_getline(std::istream& in, type& t, const char& delimiter)
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
    bool safe_getline<std::string>(std::istream& in, std::string& s, const char& delimiter)
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
            unsigned int x(0);
            for(; ((x < sizeof(type)) && in.good()); ++x)
            {
                mem[x] = in.get();
            }
            if(((x + 1) < sizeof(type)) && !in.fail())
            {
                in.setstate(std::ios_base::failbit);
            }
            memcpy(&t, mem, sizeof(type));
        }
        delete[] mem;
        return in;
    }
    
    template std::istream& in_mem(std::istream&, unsigned long long&);
    template std::istream& in_mem(std::istream&, long long&);
    template std::istream& in_mem(std::istream&, std::size_t&);
    template std::istream& in_mem<unsigned int>(std::istream&, unsigned int&);
    
    template<typename type>
    std::ostream& out_mem(std::ostream& out, const type& t)
    {
        char *mem(new char[sizeof(type)]);
        
        memcpy(mem, &t, sizeof(type));
        if(out.good())
        {
            unsigned int x(0);
            for(; ((x < sizeof(type)) && out.good()); ++x)
            {
                out<< mem[x];
            }
            if(((x + 1) < sizeof(type)) && !out.fail()) out.setstate(std::ios_base::failbit);
        }
        delete[] mem;
        return out;
    }
    
    template std::ostream& out_mem(std::ostream&, const unsigned long long&);
    template std::ostream& out_mem(std::ostream&, const long long&);
    template std::ostream& out_mem(std::ostream&, const std::size_t&);
    template std::ostream& out_mem<unsigned int>(std::ostream&, const unsigned int&);
    
    /**
     * @brief Writes a vector of type 'type' to a stream.  Garunteed not to
     * cause delimiter collisions.  Intended to use in operators.
     * @param out Stream to write to.
     * @param v The vector to write.
     * @return The stream.
     */
    template<typename type>
    std::ostream& write_vector(std::ostream& out, const std::vector<type>& v)
    {
        if(out.good())
        {
            out_mem<std::size_t>(out, v.size());
            for(unsigned int x(0); ((x < v.size()) && out.good()); ++x)
            {
                out<< v[x];
            }
        }
        return out;
    }
    
    template std::ostream& write_vector<data::budget_data>(std::ostream&, const std::vector<data::budget_data>&);
    template std::ostream& write_vector<data::money_alloc_data>(std::ostream&, const std::vector<data::money_alloc_data>&);
    
    /**
     * @brief Reads a vector of type 'type' from a stream.  Garunteed not to
     * cause delimiter collisions. Intended to use in operators.
     * @param in The stream to read from.
     * @param v The vector to store the data in.
     * @return The stream.
     */
    template<typename type>
    std::istream& read_vector(std::istream& in, std::vector<type>& v)
    {
        v.erase(v.begin(), v.end());
        in.peek();
        if(in.good())
        {
            std::size_t size(0);
            
            in_mem<std::size_t>(in, size);
            for(std::size_t x(0); ((x < size) && in.good() && (in.peek() != EOF)); ++x)
            {
                v.push_back(type());
                in>> v.back();
                if(in.fail() && !in.eof()) v.pop_back();
                in.peek();
            }
        }
        return in;
    }
    
    template std::istream& read_vector<data::budget_data>(std::istream&, std::vector<data::budget_data>&);
    template std::istream& read_vector<data::money_alloc_data>(std::istream&, std::vector<data::money_alloc_data>&);
    
    /**
     * @brief Peeks 'count' characters.  Can be used to check multi-char delimiters.
     * The stream is garunteed to be left in the state it was found.
     * @param in stream to peek
     * @param count number of characters to peek.
     * @return A string of the next 'count' characters.  Not garunteed to
     * be of size 'count'.  If the end of the stream is reached before 'count'
     * characters can be read, the operation is aborted, and the string is returned.
     */
    std::string peek_string(std::istream& in, const unsigned int& count)
    {
        std::string temps;
        
        in.peek();
        if(in.good())
        {
            std::istream::pos_type pos(in.tellg());
            for(unsigned int x(0); ((x < count) && (in.peek() != EOF) && in.good()); ++x) temps += in.get();
            if(!in.good()) in.clear();
            in.seekg(pos);
        }
        return temps;
    }
    
    
}