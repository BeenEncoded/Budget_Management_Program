#ifndef MENU_SUBMENU_MODIFY_TIMEFRAME_HPP_INCLUDED
#define MENU_SUBMENU_MODIFY_TIMEFRAME_HPP_INCLUDED
#include <utility>

#include "utility/time_frame.hpp"

namespace menu
{
    std::pair<bool, bool> modify_interval(tdata::time_interval_type&);
    std::pair<bool, bool> modify_timeframe(tdata::timeframe_class&);
    
}

#endif