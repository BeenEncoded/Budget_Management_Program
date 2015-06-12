#ifndef ALLOCATION_DISTRIBUTION_MENU_HPP_INCLUDED
#define ALLOCATION_DISTRIBUTION_MENU_HPP_INCLUDED


#include "data/budget_data.hpp"

namespace submenu
{
    bool distribution_selection(data::budget_data&);
    bool distribute_by_percent(data::budget_data&);
}

#endif