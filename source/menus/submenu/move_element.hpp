#ifndef MENU_SUBMENU_MOVE_ELEMENT_HPP_INCLUDED
#define MENU_SUBMENU_MOVE_ELEMENT_HPP_INCLUDED
#include <vector>
#include <functional>

#include "utility/scroll_display.hpp"

namespace submenu
{
    template<typename type1, typename type2 = void()> bool move_vector_element(
            scrollDisplay::window_data_class<type1>&, 
            std::vector<type1>&, 
            const std::function<type2>* const update = nullptr);
    
}

#endif