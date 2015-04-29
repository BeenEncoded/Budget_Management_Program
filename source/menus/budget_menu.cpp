#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <utility>

#include "budget_menu.hpp"
#include "utility/scroll_display.hpp"
#include "utility/user_input.hpp"
#include "common/common.hpp"
#include "common/global/global_defines.hpp"
#include "data/budget_data.hpp"
#include "utility/scroll_display.hpp"
#include "utility/filesystem.hpp"
#include "utility/stream_operations.hpp"

namespace
{
    std::pair<tdata::time_class, data::money_t> load_basic_info(const std::string&);
    void create_display(const std::vector<std::string>&, std::vector<std::string>&);
    bool usr_delete_file(const std::string&);
    void display_error(const std::string&);
    bool call_budget_mod(const std::string&);
    
    
    
    inline std::pair<tdata::time_class, data::money_t> load_basic_info(const std::string& path)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        
        std::pair<tdata::time_class, data::money_t> tempdat;
        
        if(is_file(path).value && !is_symlink(path).value)
        {
            data::budget_data tempbud;
            if(common::load_from_file(path, tempbud))
            {
                tempdat.first = std::move(tempbud.timestamp);
                tempdat.second = std::move(tempbud.total_money);
            }
            tempbud.allocs.erase(tempbud.allocs.begin(), tempbud.allocs.end());
            tempbud.allocs.shrink_to_fit();
        }
        return tempdat;
    }
    
    inline void create_display(const std::vector<std::string>& paths, std::vector<std::string>& display)
    {
        display.erase(display.begin(), display.end());
        
        std::pair<tdata::time_class, data::money_t> tempdat;
        
        for(unsigned int x(0); x < paths.size(); ++x)
        {
            tempdat = std::move(load_basic_info(paths[x]));
            display.emplace_back((common::date_disp(tempdat.first) + "  ($" + 
                    std::to_string(tempdat.second / 100) + "."));
            if((tempdat.second % 100) < 10) display.back() += "0";
            display.back() += std::to_string((tempdat.second % 100));
            display.back() += ")";
        }
    }
    
    /**
     * @brief Deletes a file, but displays any errors to the user.  This is used
     * for menu operations.
     * @param path the path to delete (will not delete folders with sub-paths)
     * @return true if the path does't exist.
     */
    inline bool usr_delete_file(const std::string& path)
    {
        using std::cout;
        using std::endl;
        
        if(fsys::is_file(path).value)
        {
            fsys::result_data_boolean tempres(fsys::fdelete(path));
            if(!tempres.value) display_error(tempres.error);
            return tempres.value;
        }
        return !fsys::is_file(path).value;
    }
    
    inline void display_error(const std::string& mess)
    {
        using std::cout;
        using std::endl;
        
        common::cls();
        cout<< endl;
        common::center("Error occured: ");
        for(unsigned int x(0); x < 5; ++x) cout<< endl;
        cout<< mess<< endl;
        common::wait();
        common::cls();
    }
    
    /**
     * @brief Calls modify_budget, but also handles the loading
     * and saving of the budget, as well as displaying any errors that occur.
     * This essentially seperates the loading/saving code from the menus.  Menus
     * should not save or load any data.
     * @param path the path chosen.
     * @return true if no errors occured.
     */
    inline bool call_mod_budget(const std::string& path)
    {
        bool success(true);
        data::budget_data tempbud;
        if(common::load_from_file(path, tempbud))
        {
            common::result_data<bool> tempres(menu::modify_budget(tempbud));
            if(tempres.success && !tempres.data)
            {
                if(!common::save_to_file(path, tempbud))
                {
                    success = false;
                    display_error("Could not save budget!");
                }
            }
            else if(!tempres.success) success = false;
        }
        else
        {
            success = false;
            display_error("Could not load file \"" + path + "\"!");
        }
        return success;
    }
    
    
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
        using scrollDisplay::window_data_class;
        using scrollDisplay::display_window;
        using std::cout;
        using std::endl;
        
        window_data_class<std::string> scroll_window(pdat.budget_files, create_display);
        bool finished(false), graceful_run(true);
        keyboard::key_code_data key;
        
        pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
        do
        {
            common::cls();
            cout<< endl;
            common::center("Budgets: ");
            for(unsigned int x(0); x < 3; ++x) cout<< endl;
            display_window(scroll_window);
            cout<< endl<< endl;
            cout<< "\'a\' -  Add new budget"<< endl;
            cout<< "\'q\' -  Quit"<< endl;
            
            key = std::move(user_input::gkey_funct());
            {
                using namespace keyboard::code;
                using keyboard::keys;
                
                if(keyboard::is_listed_control(key))
                {
                    if(key == keys[up::value]) scroll_window.win().mv_up();
                    else if(key == keys[down::value]) scroll_window.win().mv_down();
                    else if(key == keys[home::value]) scroll_window.win().jmp(0);
                    else if(key == keys[end::value]) scroll_window.win().jmp((scroll_window.gdata()->size() - 1));
                    else if(key == keys[del::value])
                    {
                        if(common::prompt_user("Are you sure you want to delete the \
budget for " + common::date_disp(load_basic_info(scroll_window.selected()).first) + "?  This \
is permanent!"))
                        {
                            if(usr_delete_file(scroll_window.selected()))
                            {
                                scroll_window.remove_selected();
                            }
                        }
                    }
                }
                else
                {
                    if(!key.control_d.empty())
                    {
                        switch(tolower((char)key.control_d.front()))
                        {
                            case '\n':
                            {
                                call_mod_budget(scroll_window.selected());
                            }
                            break;
                            
                            case 'a':
                            {
                                //cur_pos
                            }
                            break;
                            
                            case 'q':
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
                }
            }
        }while(!finished);
        return graceful_run;
    }
    
    common::result_data<bool> modify_budget(data::budget_data& b)
    {
        /* result's defined boolean represents whether the user
         * canceled modification. */
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