#ifndef MENUS_BUDGET_MENU_HPP_INCLUDED
#define MENUS_BUDGET_MENU_HPP_INCLUDED
#include <utility>

#include "common/common.hpp"
#include "common/global/program_data.hpp"
#include "data/budget_data.hpp"

namespace menu
{
    bool budget_list_menu(global::program_data&);
    std::pair<bool, bool> modify_budget(global::program_data&, data::budget_data&);
    std::pair<bool, bool> modify_allocation(data::budget_data&, data::money_alloc_data&);
    
}

#endif