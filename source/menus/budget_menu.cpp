#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <fstream>

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
    data::budget_data load_basic_info(const std::string&);
    void create_display(const std::vector<std::string>&, std::vector<std::string>&);
    bool usr_delete_file(const std::string&);
    void display_error(const std::string&);
    bool call_mod_budget(const std::string&);
    bool call_create_budget(global::program_data&);
    std::vector<data::budget_data> load_basic_info_all(global::program_data&);
    
    
    /**
     * @brief Loads basic info of finite size.  Used for memory efficiency.
     * @return a budget data.
     */
    inline data::budget_data load_basic_info(const std::string& path)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        using utility::in_mem;
        
        data::budget_data tempbud;
        
        if(is_file(path).value && !is_symlink(path).value)
        {
            std::ifstream in(path.c_str(), std::ios::in);
            if(in.good()) in_mem(in, tempbud.total_money);
            if(in.good()) in_mem(in, tempbud.id);
            if(in.good()) in>> tempbud.timestamp;
            in.close();
        }
        return tempbud;
    }
    
    /**
     * @brief Used to created a display vector for a window_data_class.
     */
    inline void create_display(const std::vector<std::string>& paths, std::vector<std::string>& display)
    {
        display.erase(display.begin(), display.end());
        
        data::budget_data tempbud;
        
        for(unsigned int x(0); x < paths.size(); ++x)
        {
            tempbud = std::move(load_basic_info(paths[x]));
            display.emplace_back((common::date_disp(tempbud.timestamp) + "  ($" + 
                    std::to_string(tempbud.total_money / 100) + "."));
            if((tempbud.total_money % 100) < 10) display.back() += "0";
            display.back() += std::to_string((tempbud.total_money % 100));
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
     * @return true if the budget was modified.
     */
    inline bool call_mod_budget(const std::string& path)
    {
        bool success(true);
        data::budget_data tempbud;
        
        if(common::load_from_file(path, tempbud))
        {
            std::pair<bool, bool> tempres(menu::modify_budget(tempbud));
            if(tempres.first && !tempres.second)
            {
                if(!common::save_to_file(path, tempbud))
                {
                    success = false;
                    display_error("Could not save budget!");
                }
            }
            else if(!tempres.first) success = false;
        }
        else
        {
            success = false;
            display_error("Could not load file \"" + path + "\"!");
        }
        return success;
    }
    
    inline std::vector<data::budget_data> load_basic_info_all(global::program_data& pdata)
    {
        pdata.budget_files = std::move(global::budget_paths(pdata.budget_folder));
        if(pdata.budget_files.empty()) return std::vector<data::budget_data>();
        
        std::vector<data::budget_data> tempv;
        
        for(std::vector<std::string>::const_iterator it{pdata.budget_files.begin()}; it != pdata.budget_files.end(); ++it)
        {
            tempv.push_back(std::move(load_basic_info(*it)));
        }
        return tempv;
    }
    
    inline bool call_create_budget(global::program_data& pdata)
    {
        data::budget_data tempb;
        tempb.id = std::move(data::new_budget_id(load_basic_info_all(pdata)));
        tempb.timestamp = tdata::current_time();
        //cur_pos
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
budget for " + common::date_disp(load_basic_info(scroll_window.selected()).timestamp) + "?  This \
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
                                //todo
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
    
    std::pair<bool, bool> modify_budget(data::budget_data& b)
    {
        /* the result is two bools: first = whether is was modified
         * second = whether the modification was canceled by the user. */
        std::pair<bool, bool> result{false, false};
        
        return result;
    }
    
    
}