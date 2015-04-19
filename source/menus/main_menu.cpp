#include <iostream>
#include <utility>
#include <fstream>
#include <string>

#include "main_menu.hpp"
#include "utility/time_class.hpp"
#include "common/global/global_defines.hpp"
#include "utility/user_input.hpp"
#include "common/common.hpp"
#include "common/global/program_data.hpp"
#include "data/budget_data.hpp"
#include "utility/filesystem.hpp"

namespace
{
    common::result_data<std::pair<tdata::time_class, data::money_t> > basic_info(const std::string&);
    
    
    
    
    /**
     * @brief Loads basic information about a budget, given its path.
     * @return a std::pair<tdata::time_class, data::money_t>
     */
    inline common::result_data<std::pair<tdata::time_class, data::money_t> > basic_info(const std::string& s)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        
        common::result_data<std::pair<tdata::time_class, data::money_t> > result;
        
        result.message = "ERROR IN common::result_data<std::pair<tdata::time_class, \
data::money_t> > basic_info(const std::string&): ";
        
        if(is_file(s).value && !is_symlink(s).value)
        {
            std::ifstream in(s.c_str(), std::ios::in);
            if(in.good())
            {
                common::in_mem(in, result.data.second);
                in.peek();
                if(in.good()) in>> result.data.first;
                if(!in.fail()) result.success = true;
                else result.message += "Stream read failure.";
            }
            else
            {
                result.message += "Stream could not be opened!";
            }
            if(in.is_open()) in.close();
        }
        else
        {
            result.message += ("\"" + s + "\" is not a valid path!  Unable to \
load basic data.");
        }
        if(result.success) result.message.erase();
        return result;
    }
    
    
}

namespace menu
{
    /**
     * @brief The main menu to the budget program.
     * @param pdat program data.
     * @return true if no errors were encountered.
     */
    bool main_menu(global::program_data& pdat)
    {
        using common::result_data;
        using global::program_data;
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        bool finished(false);
        key_code_data key;
        
        do
        {
            common::cls();
            //todo finish
        }while(!finished);
        return true;
    }
    
    
}