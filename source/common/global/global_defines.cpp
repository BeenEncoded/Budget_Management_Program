#include <vector>
#include <iostream>
#include <string>
#include <unistd.h>
#include <termios.h>

#include "global_defines.hpp"

namespace
{
    std::vector<key_code::key_code_data> default_keys();
    
    
    
    inline std::vector<key_code::key_code_data> default_keys()
    {
        using key_code::key_code_data;
        
        return std::vector<key_code::key_code_data>({
            key_code_data((char)8), //backspace
            key_code_data((char)27),  //escape
            key_code_data(std::vector<int>({27, 91, 51, 126})), //delete
            key_code_data(std::vector<int>({27, 91, 50, 126})), //insert
            key_code_data(std::vector<int>({27, 79, 70})),      //end
            key_code_data(std::vector<int>({27, 79, 72})),      //home
            key_code_data(std::vector<int>({27, 91, 65})),      //up
            key_code_data(std::vector<int>({27, 91, 66})),      //down
            key_code_data(std::vector<int>({27, 91, 68})),      //left
            key_code_data(std::vector<int>({27, 91, 67})),      //right
            key_code_data(std::vector<int>({27, 91, 53, 126})), //pgup
            key_code_data(std::vector<int>({27, 91, 54, 126})), //pgdown
            key_code_data((char)0x00), //                       f1 key: could not reproduce; reason: terminal grabbed it
            key_code_data(std::vector<int>({27, 79, 81})), //     f2
            key_code_data(std::vector<int>({27, 79, 82})), //     f3
            key_code_data(std::vector<int>({27, 79, 83})), //     f4
            key_code_data(std::vector<int>({27, 91, 49, 53, 126})), //f5
            key_code_data(std::vector<int>({27, 91, 49, 55, 126})), //f6
            key_code_data(std::vector<int>({27, 91, 49, 56, 126})), //f7
            key_code_data(std::vector<int>({27, 91, 49, 57, 126})), //f8
            key_code_data(std::vector<int>({27, 91, 50, 48, 126})), //f9
            key_code_data((char)0x00), //could not reproduce F10: terminal grabbed it
            key_code_data((char)0x00), //could not reproduce F11: terminal grabbed it
            key_code_data(std::vector<int>({27, 91, 50, 52, 126})) //f12
        });
    }
    
    
}

namespace key_code
{
    
    std::vector<key_code_data> keys(default_keys());
    
    /* allows display of key code*/
    std::ostream& operator<<(std::ostream& out, const key_code_data& k)
    {
        if(out.good())
        {
            if(k.is_control)
            {
                out<< '{';
                for(unsigned int x = 0; x < k.control_d.size(); x++)
                {
                    out<< k.control_d[x];
                    if(x < (k.control_d.size() - 1)) out<< ' ';
                }
                out<< '}';
            }
            else
            {
                out<< (k.control_d.empty() ? char(0x00) : (char)k.control_d.front());
            }
        }
        return out;
    }
    
    key_code_data::key_code_data(const char& c) : is_control(false), control_d({(int)c})
    {
    }
    
    key_code_data::key_code_data(const std::vector<int>& c) : is_control(true), control_d(c)
    {
    }
    
    key_code_data::key_code_data() : is_control(false), control_d()
    {
    }
    
    key_code_data::~key_code_data()
    {
        this->control_d.clear();
        this->control_d.shrink_to_fit();
    }
    
    const key_code_data& key_code_data::operator=(const key_code_data& k)
    {
        if(this != &k)
        {
            this->is_control = k.is_control;
            this->control_d = k.control_d;
        }
        return *this;
    }
    
    bool key_code_data::operator==(const key_code_data& k) const
    {
        return (
                    (this->is_control == k.is_control) &&
                    (this->control_d == k.control_d));
    }
    
    bool key_code_data::operator!=(const key_code_data& k) const
    {
        return !(this->operator==(k));
    }
    
    bool is_listed_control(const key_code_data& k)
    {
        for(std::vector<key_code_data>::const_iterator it = keys.begin(); it != keys.end(); ++it)
        {
            if(*it == k) return true;
        }
        return false;
    }
    
    bool might_be_control(const key_code_data& k)
    {
        if(k.is_control)
        {
            if(!k.control_d.empty())
            {
                return (k.control_d[0] == 27);
            }
        }
        return false;
    }
    
    
}