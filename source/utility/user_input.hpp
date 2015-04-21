#ifndef USER_OPTIONS_HPP_INCLUDED
#define USER_OPTIONS_HPP_INCLUDED
#include <string>
#include <vector>
#include <utility>
#include <iostream>

namespace keyboard
{
    typedef class key_code_data key_code_data;
    
    std::ostream& operator<<(std::ostream&, const keyboard::key_code_data&);
    
    bool is_listed_control(const keyboard::key_code_data&);
    bool might_be_control(const keyboard::key_code_data&);
    std::vector<keyboard::key_code_data> default_keys();
    
    extern std::vector<keyboard::key_code_data> keys;
    
    class key_code_data
    {
    public:
        key_code_data(const keyboard::key_code_data&) noexcept;
        key_code_data(keyboard::key_code_data&&) noexcept;
        explicit key_code_data(const std::vector<int>&);
        explicit key_code_data(const char&);
        explicit key_code_data();
        
        ~key_code_data();
        
        keyboard::key_code_data& operator=(const keyboard::key_code_data&);
        keyboard::key_code_data& operator=(keyboard::key_code_data&&) noexcept;
        
        bool operator==(const keyboard::key_code_data&) const;
        bool operator!=(const keyboard::key_code_data&) const;
        
        //I try to make this as small as possible:
        bool is_control : 1;
        
        std::vector<int> control_d;
    };
    
    namespace code
    {
        template<const unsigned int>
        struct const_int_type
        {
            static const unsigned int value;
        };
        
        template<const unsigned int i> const unsigned int const_int_type<i>::value = i;
        
        typedef const_int_type<0> backspace;
        typedef const_int_type<1> escape;
        typedef const_int_type<2> del;
        typedef const_int_type<3> insert;
        typedef const_int_type<4> end;
        typedef const_int_type<5> home;
        typedef const_int_type<6> up;
        typedef const_int_type<7> down;
        typedef const_int_type<8> left;
        typedef const_int_type<9> right;
        typedef const_int_type<10> pgup;
        typedef const_int_type<11> pgdown;
        typedef const_int_type<12> f1;
        typedef const_int_type<13> f2;
        typedef const_int_type<14> f3;
        typedef const_int_type<15> f4;
        typedef const_int_type<16> f5;
        typedef const_int_type<17> f6;
        typedef const_int_type<18> f7;
        typedef const_int_type<19> f8;
        typedef const_int_type<20> f9;
        typedef const_int_type<21> f10;
        typedef const_int_type<22> f11;
        typedef const_int_type<23> f12;
    }
}


namespace user_input
{
    template<typename type> struct menu_option_data;
    
    keyboard::key_code_data getch_funct();
    char gkey();
    keyboard::key_code_data gkey_funct();
    bool kbhit();
    void cl();

    /**
     * @class menu_option_data
     * @author Jonathan Whitlock
     * @date 04/19/2015
     * @file user_input.hpp
     * @brief Stores a menu option.  Each option is associated with the description of
     * the option, the actual key code, and a pointer to the function that
     * should be executed, should the user choose the option.
     */
    template<typename type>
    struct menu_option_data
    {
        typedef type (*funct_type)(...);
        
        explicit menu_option_data();
        explicit menu_option_data(const funct_type&, const std::string&, const keyboard::key_code_data&);
        explicit menu_option_data(funct_type&, const std::string&, const keyboard::key_code_data&);
        menu_option_data(const menu_option_data<type>&);
        menu_option_data(menu_option_data<type>&&) noexcept;
        
        ~menu_option_data();
        
        menu_option_data<type>& operator=(const menu_option_data<type>&);
        menu_option_data<type>& operator=(menu_option_data<type>&&) noexcept;
        
        funct_type exec;
        std::string description;
        keyboard::key_code_data key;
    };
    
    
    
    template<typename type> typename menu_option_data<type>::funct_type get_input(std::vector<menu_option_data<type> >&);
    
    
}

#endif