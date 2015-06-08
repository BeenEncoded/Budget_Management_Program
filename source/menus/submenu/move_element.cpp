#include <iostream>
#include <vector>
#include <utility>

#include "move_element.hpp"
#include "utility/scroll_display.hpp"
#include "utility/user_input.hpp"
#include "common/common.hpp"
#include "data/budget_data.hpp"

namespace
{
    template<typename type> bool move_element_up(std::vector<type>&, const std::size_t&);
    template<typename type> bool move_element_down(std::vector<type>&, const std::size_t&);
    
    
    
    
    template<typename type>
    inline bool move_element_up(std::vector<type>& v, const std::size_t& pos)
    {
        if(v.empty()) return false;
        if(v.size() == 1) return false;
        if(pos > 0)
        {
            std::swap(v[pos], v[(pos - 1)]);
            return true;
        }
        return false;
    }
    
    template<typename type>
    inline bool move_element_down(std::vector<type>& v, const std::size_t& pos)
    {
        if(v.empty()) return false;
        if(v.size() == 1) return false;
        if(pos < (v.size() - 1))
        {
            std::swap(v[pos], v[(pos + 1)]);
            return true;
        }
        return false;
    }
    
    
}

namespace submenu
{
    /**
     * @brief Allows the user to move a selected element's position in a vector.
     * @param window the scroll_display to use.  This is needed because the 
     * vector is visually represented.
     * @param v The vector.
     * @return true if the user didn't cancel.
     */
    template<typename type>
    bool move_vector_element(scrollDisplay::window_data_class<type>& window, std::vector<type>& v)
    {
        using scrollDisplay::window_data_class;
        using scrollDisplay::display_window;
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        bool finished{false}, cancel{false};
        std::size_t original_pos{window.win().gpos().whole};
        key_code_data key;
        
        user_input::cl();
        do
        {
            common::cls();
            cout<< endl;
            common::center("Move Selection:");
            for(unsigned int x{0}; x < 4; ++x) cout<< endl;
            display_window(window, std::pair<char, char>{'>', '<'});
            
            key = std::move(user_input::getch_funct());
            if(keyboard::is_listed_control(key))
            {
                using keyboard::keys;
                using namespace keyboard::code;
                
                if(key == keys[up::value])
                {
                    move_element_up(v, window.win().gpos().whole);
                    window.win().mv_up();
                }
                else if(key == keys[down::value])
                {
                    move_element_down(v, window.win().gpos().whole);
                    window.win().mv_down();
                }
                else if(key == keys[end::value])
                {
                    bool moved{false};
                    do
                    {
                        moved = move_element_down(v, window.win().gpos().whole);
                        if(moved) moved = window.win().mv_down();
                    }while(moved);
                }
                else if(key == keys[home::value])
                {
                    bool moved{false};
                    do
                    {
                        moved = move_element_up(v, window.win().gpos().whole);
                        if(moved) moved = window.win().mv_up();
                    }while(moved);
                }
                else if(key == keys[escape::value])
                {
                    cancel = true;
                    finished = true;
                }
            }
            else if(!key.control_d.empty())
            {
                switch(std::tolower((char)key.control_d.front()))
                {
                    case '\n':
                    {
                        finished = true;
                    }
                    break;
                    
                    default:
                    {
                    }
                    break;
                }
            }
        }while(!finished);
        if(cancel)
        {
            type temp_t{std::move(v[window.win().gpos().whole])};
            std::size_t temp_pos{window.win().gpos().whole};
            
            v.erase((v.begin() + temp_pos));
            v.insert((v.begin() + original_pos), temp_t);
        }
        return !cancel;
    }
    
    template bool move_vector_element<data::money_alloc_data>(scrollDisplay::window_data_class<data::money_alloc_data>&, std::vector<data::money_alloc_data>&);
    
    
}