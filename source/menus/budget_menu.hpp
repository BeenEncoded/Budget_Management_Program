#ifndef MENUS_BUDGET_MENU_HPP_INCLUDED
#define MENUS_BUDGET_MENU_HPP_INCLUDED

#include "common/common.hpp"
#include "common/global/program_data.hpp"
#include "data/budget_data.hpp"

namespace menu
{
    bool budget_list_menu(global::program_data&);
    common::result_data<bool> modify_budget(data::budget_data&);
    
}

#endif