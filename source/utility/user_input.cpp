#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <unistd.h>
#include <cstdio>
#include <termios.h>
#include <fcntl.h>

#include "user_input.hpp"

namespace
{
    keyboard::key_code_data getch();
    
    
    inline keyboard::key_code_data getch()
    {
        int ch;
        struct termios temp_term, cur_term;
        tcgetattr(STDIN_FILENO, &cur_term);
        temp_term = cur_term;
        temp_term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &temp_term);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);
        
        return keyboard::key_code_data(char(ch));
    }
    
    
}

namespace keyboard
{
    std::vector<keyboard::key_code_data> keys(default_keys());
    
    std::vector<keyboard::key_code_data> default_keys()
    {
        return std::vector<keyboard::key_code_data>({
            key_code_data((char)8), //backspace
            key_code_data((char)27),  //escape
            key_code_data(std::vector<int>({27, 91, 51, 126})), //delete
            key_code_data(std::vector<int>({27, 91, 50, 126})), //insert
            key_code_data(std::vector<int>({27, 91, 70})),      //end
            key_code_data(std::vector<int>({27, 91, 72})),      //home
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
    
    /* allows display of key code*/
    std::ostream& operator<<(std::ostream& out, const keyboard::key_code_data& k)
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
    
    bool is_listed_control(const keyboard::key_code_data& k)
    {
        for(std::vector<keyboard::key_code_data>::const_iterator it = keys.begin(); it != keys.end(); ++it)
        {
            if(*it == k) return true;
        }
        return false;
    }
    
    bool might_be_control(const keyboard::key_code_data& k)
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
    
    key_code_data::key_code_data(const keyboard::key_code_data& k) noexcept : 
            is_control(k.is_control),
            control_d(k.control_d)
    {
    }
    
    key_code_data::key_code_data(keyboard::key_code_data&& k) noexcept : 
            is_control(k.is_control),
            control_d(std::move(k.control_d))
    {
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
    
    keyboard::key_code_data& key_code_data::operator=(keyboard::key_code_data&& k) noexcept
    {
        this->is_control = k.is_control;
        this->control_d = std::move(k.control_d);
        return *this;
    }
    
    keyboard::key_code_data& key_code_data::operator=(const keyboard::key_code_data& k)
    {
        if(this != &k)
        {
            this->is_control = k.is_control;
            this->control_d = k.control_d;
        }
        return *this;
    }
    
    bool key_code_data::operator==(const keyboard::key_code_data& k) const
    {
        return (
                    (this->is_control == k.is_control) &&
                    (this->control_d == k.control_d));
    }
    
    bool key_code_data::operator!=(const keyboard::key_code_data& k) const
    {
        return !(this->operator==(k));
    }
    
    
}


namespace user_input
{
    template<typename type>
    menu_option_data<type>::menu_option_data() : 
            exec(nullptr),
            description(),
            key()
    {
    }

    template<typename type>
    menu_option_data<type>::menu_option_data(const menu_option_data<type>::funct_type& f,
            const std::string& n, const keyboard::key_code_data& k) : 
                    exec(f),
                    description(n),
                    key(k)
    {
    }

    template<typename type>
    menu_option_data<type>::menu_option_data(const menu_option_data<type>& m) : 
            exec(m.exec),
            description(m.description),
            key(m.key)
    {
    }

    template<typename type>
    menu_option_data<type>::menu_option_data(menu_option_data<type>&& m) noexcept : 
            exec(std::move(m.exec)),
            description(std::move(m.description)),
            key(std::move(m.key))
    {
    }

    template<typename type>
    menu_option_data<type>::~menu_option_data()
    {
    }

    template<typename type>
    menu_option_data<type>& menu_option_data<type>::operator=(const menu_option_data<type>& m)
    {
        if(this != &m)
        {
            this->exec = m.exec;
            this->description = m.description;
            this->key = m.key;
        }
        return *this;
    }

    template<typename type>
    menu_option_data<type>& menu_option_data<type>::operator=(menu_option_data<type>&& m) noexcept
    {
        this->exec = std::move(m.exec);
        this->description = std::move(m.description);
        this->key = std::move(m.key);
        return *this;
    }


}

namespace user_input
{
    /**
     * @brief gets a user's key press.  Does not clear the buffer.
     * @return The key a user has pressed.
     */
    keyboard::key_code_data getch_funct()
    {
        using keyboard::key_code_data;
        using keyboard::is_listed_control;
        using keyboard::might_be_control;
        
        key_code_data key;
        
        key.is_control = true;
        do
        {
            key.control_d.emplace_back(getch().control_d[0]);
        }while(might_be_control(key) && kbhit());
        key.is_control = (key.control_d.size() > 1);
        return key;
    }
    
    char gkey()
    {
        cl();
        return getch().control_d[0];
    }
    
    keyboard::key_code_data gkey_funct()
    {
        cl();
        return getch_funct();
    }
    
    bool kbhit()
    {
        int ch, oldf;
        bool button_hit(false);
        struct termios temp_term, cur_term;
        
        tcgetattr(STDIN_FILENO, &cur_term);
        temp_term = cur_term;
        temp_term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &temp_term);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF)
        {
            ungetc(ch, stdin);
            button_hit = true;
        }
        return button_hit;
    }
    
    void cl()
    {
        int ch, oldf;
        struct termios cur_term, new_term;
        tcgetattr(STDIN_FILENO, &cur_term);
        new_term = cur_term;
        new_term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        do
        {
            ch = getchar();
        }
        while(ch != EOF);
        ungetc(ch, stdin);
        
        tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
    }
    
    template<typename type>
    typename menu_option_data<type>::funct_type get_input(std::vector<menu_option_data<type> >& options)
    {
        if(options.empty()) return nullptr;
        //cur_pos
        return nullptr;
    }
    
    
}