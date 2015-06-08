#ifndef MENU_SUBMENU_MOVE_ELEMENT_HPP_INCLUDED
#define MENU_SUBMENU_MOVE_ELEMENT_HPP_INCLUDED
#include <vector>

#include "utility/scroll_display.hpp"

namespace submenu
{
    template<typename type> bool move_vector_element(scrollDisplay::window_data_class<type>&, std::vector<type>&);
    
    
}

#endif