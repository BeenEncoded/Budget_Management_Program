#include <iostream>

#include "budget_menu.hpp"
#include "utility/scroll_display.hpp"
#include "common/common.hpp"
#include "common/global/global_defines.hpp"
#include "data/budget_data.hpp"

namespace
{
}

namespace menu
{
    /**
     * @brief Allows the user to modify any of the budgets found at load.  This
     * menu also allows the user to create new budgets.
     * @param pdat program data
     * @return true if no errors occured.
     */
    bool budget_list_menu(global::program_data& pdat)
    {
        return true; //todo
    }
    
    common::result_data<bool> modify_budget(data::budget_data& b)
    {
        common::result_data<bool> result;
        
        result.message = "ERROR IN common::result_data<bool> modify_budget(dat\
a::budget_data&): ";
        result.data = false;
        
        //todo (this function returns success until I get to work on it
        result.success = true;
        result.message.erase();
        
        return result;
    }
    
    
}