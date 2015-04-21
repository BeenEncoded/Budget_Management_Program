#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <string>

#include "main_menu.hpp"
#include "utility/time_class.hpp"
#include "utility/user_input.hpp"
#include "common/common.hpp"
#include "common/global/program_data.hpp"
#include "data/budget_data.hpp"
#include "utility/user_input.hpp"
#include "utility/filesystem.hpp"

namespace
{
    common::result_data<std::pair<tdata::time_class, data::money_t> > basic_info(const std::string&);
    std::vector<user_input::menu_option_data<void> > main_menu_options();
    
    
    
    
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
    
    inline std::vector<user_input::menu_option_data<void> > main_menu_options()
    {
        using user_input::menu_option_data;
        using keyboard::key_code_data;
        using keyboard::keys;
        
        return std::vector<menu_option_data<void> >{
            menu_option_data<void>{nullptr, "e -  Exit", key_code_data{'e'}}
        };
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
        using std::cout;
        using user_input::menu_option_data;
        using keyboard::key_code_data;
        using std::endl;
        
        bool finished(false);
        key_code_data key;
        std::vector<menu_option_data<void> > options{main_menu_options()};
        typename menu_option_data<void>::funct_type exec;
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Main menu");
            for(unsigned int x = 0; x < 4; ++x) cout<< endl;
            
            for(unsigned int x(0); x < options.size(); ++x)
            {
                if(options[x].key == key_code_data{'e'}) cout<< endl<< endl;
                cout<< options[x].description;
                if(x < (options.size() - 1)) cout<< endl;
            }
            exec = std::move(user_input::get_input(options));
            
            if(exec != nullptr)
            {
                exec((global::program_data*)(&pdat));
            }
            else finished = true;
        }while(!finished);
        return true;
    }
    
    
}