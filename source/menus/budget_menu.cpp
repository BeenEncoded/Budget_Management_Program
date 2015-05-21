#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <fstream>

#include "budget_menu.hpp"
#include "utility/scroll_display.hpp"
#include "utility/user_input.hpp"
#include "common/common.hpp"
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
    void money_alloc_list_display(const std::vector<data::money_alloc_data>&, std::vector<std::string>&);
    std::string allocation_display(const data::money_alloc_data&);
    std::string money_display(const data::money_t&);
    bool user_get_money_value(data::money_t&);
    
    
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
            if(in.is_open()) in.close();
        }
        return tempbud;
    }
    
    /**
     * @brief Used to created a display vector for a window_data_class.  Specifically,
     * it's used to create a display of a list of budgets the user has created.
     */
    inline void create_display(const std::vector<std::string>& paths, std::vector<std::string>& display)
    {
        display.erase(display.begin(), display.end());
        
        data::budget_data tempbud;
        
        for(unsigned int x(0); x < paths.size(); ++x)
        {
            tempbud = std::move(load_basic_info(paths[x]));
            display.push_back(std::move((common::date_disp(tempbud.timestamp) + "   (" + 
                    money_display(tempbud.total_money) + ")")));
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
    
    /**
     * @brief Returns a list of budgets.  This function only loads the bare info
     * necessary to identify the budgets.  This does not load additional information,
     * like allocations.
     */
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
    
    /**
     * @brief This function handles creation of a new budget, as well as saving it
     * if the user modified it.
     * @return true if a new budget was created.
     */
    inline bool call_create_budget(global::program_data& pdata)
    {
        data::budget_data tempb;
        std::pair<bool, bool> temp_result;
        bool budget_created{false};
        
        tempb.id = std::move(data::new_budget_id(load_basic_info_all(pdata)));
        //todo user should pick month before creation, and it can not be the same as a budget that already exists
        //todo user should also set total money of budget as well
        tempb.timestamp = tdata::current_time();
        temp_result = std::move(menu::modify_budget(tempb));
        if(temp_result.first && !temp_result.second) //modified, but not canceled
        {
            if(!common::save_to_file((pdata.budget_folder + fsys::pref_slash() + 
                    std::to_string(tempb.id) + std::string(global::budget_file_extension)), tempb))
            {
                display_error("Unable to save the budget!");
            }
            else
            {
                budget_created = true;
            }
        }
        return budget_created;
    }
    
    /**
     * @brief returns a string in the form "$0.00"
     */
    inline std::string money_display(const data::money_t& money)
    {
        std::string temps{"$"};
        temps += std::to_string(((long double)money / 100));
        if(temps.find('.') != std::string::npos)
        {
            if((temps.size() - temps.find('.')) > 2) temps.resize((temps.find('.') + 3));
        }
        return temps;
    }
    
    /**
     * @brief creates a string representation of a money_alloc_data.
     */
    inline std::string allocation_display(const data::money_alloc_data& a)
    {
        using common::fit_str;
        
        constexpr unsigned int name_size{20};
        
        return std::string((fit_str(a.name, name_size) + std::string((name_size - fit_str(a.name, name_size).size()), ' ') + 
                std::string(4, ' ') + money_display(a.value)));
    }
    
    /**
     * @brief Takes a list of money allocations and creates a string representation of each allocation, 
     * placing them in a list of strings.  This is used for created a window_data_class<data::money_alloc_data>.
     */
    inline void money_alloc_list_display(const std::vector<data::money_alloc_data>& allocations, std::vector<std::string>& disp)
    {
        disp.erase(disp.begin(), disp.end());
        
        for(unsigned int x{0}; x < allocations.size(); ++x) disp.push_back(std::move(allocation_display(allocations[x])));
    }
    
    /**
     * @brief Gets a monetary value from the user.
     * @param m money to modify
     * @return true if the user entered a new value.
     */
    inline bool user_get_money_value(data::money_t& m)
    {
        //todo add ability to recognize and parse simple equations
        std::string temps;
        bool modified{false}, is_valid{false};
        
        do
        {
            modified = common::get_user_str(temps);
            is_valid = common::str_is_num(temps);
        }while(modified && !is_valid);
        if(modified && is_valid)
        {
            long double tempstore{0};
            std::stringstream ss;
            ss<< temps;
            ss>> tempstore;
            m = (tempstore * 100);
        }
        return (modified && is_valid);
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
        bool finished(false);
        keyboard::key_code_data key;
        
        pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
        do
        {
            common::cls();
            cout<< endl;
            common::center("Budgets: ");
            for(unsigned int x(0); x < 3; ++x) cout<< endl;
            //todo each element in the list of budgets should display month, not whole date
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
                                pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
                            }
                            break;
                            
                            case 'a':
                            {
                                call_create_budget(pdat);
                                pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
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
        common::cls();
        return true; //there has yet to be a case where this function fails.
    }
    
    /**
     * @brief Allows the user to modify a budget.  If the user cancels, the budget
     * remains unchanged.
     * @param b The budget to modify.
     * @return an std::pair<bool, bool>.
     * result.first = whether the budget was modified,
     * result.second = whether the user canceled the operation.
     */
    std::pair<bool, bool> modify_budget(data::budget_data& b)
    {
        using scrollDisplay::window_data_class;
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        std::pair<bool, bool> result{false, false};
        data::budget_data original_budget{b};
        bool finished{false};
        window_data_class<data::money_alloc_data> scroll_display{b.allocs, money_alloc_list_display};
        key_code_data key;
        
        do
        {
            //todo add ability to modify month for budget
            //todo add ability to modify the money allocated for a budget
            common::cls();
            cout<< "Today is "<< common::date_disp(tdata::time_class{tdata::current_time()})<< endl;
            cout<< endl;
            common::center("Budget for " + b.timestamp.month_name() + ", " + std::to_string(b.timestamp.gyear()));
            for(unsigned int x{0}; x < 3; ++x) cout<< endl;
            scrollDisplay::display_window(scroll_display);
            cout<< endl<< endl;
            //todo display total money allocated
            cout<< " a -  Add new allocation"<< endl;
            cout<< " [DEL] -  Delete selected"<< endl;
            cout<< " [ENTER] -  Modify selected"<< endl;
            
            cout<< " [BCKSPC] -  Done"<< endl;
            cout<< " c -  cancel";
            cout.flush();
            
            key = std::move(user_input::gkey_funct());
            
            if(keyboard::is_listed_control(key))
            {
                using namespace keyboard::code;
                using keyboard::keys;
                
                if(key == keys[up::value]) scroll_display.win().mv_up();
                else if(key == keys[down::value]) scroll_display.win().mv_down();
                else if(key == keys[pgup::value]) scroll_display.win().pg_up();
                else if(key == keys[pgdown::value]) scroll_display.win().pg_down();
                else if(key == keys[home::value]) while(scroll_display.win().pg_up());
                else if(key == keys[end::value]) while(scroll_display.win().pg_down());
                else if(key == keys[backspace::value]) finished = true;
                else if(key == keys[del::value])
                {
                    if(!b.allocs.empty())
                    {
                        if(common::prompt_user("Do you really want to delete \"" + scroll_display.selected().name + "\"?"))
                        {
                            scroll_display.remove_selected();
                            result.first = true;
                        }
                    }
                }
            }
            else
            {
                if(key.control_d.size() == 1)
                {
                    switch(std::tolower((char)key.control_d[0]))
                    {
                        case '\n':
                        {
                            if(!b.allocs.empty())
                            {
                                data::money_alloc_data temp_allocation{scroll_display.selected()};
                                std::pair<bool, bool> temp_result{modify_allocation(temp_allocation)};
                                if(temp_result.first && !temp_result.second)
                                {
                                    scroll_display.selected() = std::move(temp_allocation);
                                    result.first = true;
                                }
                            }
                        }
                        break;
                        
                        case 'a':
                        {
                            data::money_alloc_data new_allocation{"No name", (data::money_t)0};
                            new_allocation.id = data::new_alloc_id(b.allocs);
                            std::pair<bool, bool> temp_result{modify_allocation(new_allocation)};
                            if(temp_result.first && !temp_result.second)
                            {
                                b.allocs.push_back(std::move(new_allocation));
                                result.first = true;
                            }
                        }
                        break;
                        
                        case 'c':
                        {
                            result.second = true;
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
        }while(!finished);
        if(result.second && result.first) b = original_budget;
        return result;
    }
    
    /**
     * @brief Allows the user to modify a budget allocation.
     * @param allocation The allocation to modify.
     * @return a std::pair<bool, bool>:
     * first = whether the allocation was modified
     * second = whether the user canceled modification of the allocation.
     */
    std::pair<bool, bool> modify_allocation(data::money_alloc_data& allocation)
    {
        using std::cout;
        using std::endl;
        
        std::pair<bool, bool> result{false, false};
        bool finished{false};
        keyboard::key_code_data key;
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Modify Budget Allocation");
            for(unsigned int x{0}; x < 4; ++x) cout<< endl;
            cout<< " 1 -  Name: \""<< allocation.name<< "\""<< endl;
            cout<< " 2 -  Value: "<< money_display(allocation.value)<< endl;
            cout<< endl;
            cout<< " c -  Cancel"<< endl;
            cout<< " [BCKSPC] -  Done";
            cout.flush();
            
            key = std::move(user_input::gkey_funct());
            
            if(keyboard::is_listed_control(key))
            {
                using namespace keyboard::code;
                using keyboard::keys;
                
                if(key == keys[backspace::value]) finished = true;
            }
            else
            {
                if(key.control_d.size() == 1)
                {
                    switch(std::tolower((char)key.control_d[0]))
                    {
                        case '1':
                        {
                            std::string temps;
                            common::cls();
                            cout<< "[ESC]: cancel"<< endl;
                            for(unsigned int x{0}; x < 10; ++x) cout<< endl;
                            cout<< "Name currently set to: \""<< allocation.name<< "\""<< endl;
                            cout<< endl;
                            cout<< "Enter a new name: ";
                            cout.flush();
                            if(common::get_user_str(temps))
                            {
                                allocation.name = std::move(temps);
                                result.first = true;
                            }
                            temps.erase();
                            common::cls();
                        }
                        break;
                        
                        case '2':
                        {
                            common::cls();
                            cout<< endl;
                            cout<< " [ESC]: Cancel"<< endl;
                            for(unsigned int x{0}; x < 7; ++x) cout<< endl;
                            cout<< "Currently set to: "<< money_display(allocation.value)<< endl;
                            cout<< endl;
                            cout<< "Enter the new value: ";
                            cout.flush();
                            {
                                data::money_t temp_money{allocation.value};
                                if(user_get_money_value(temp_money))
                                {
                                    allocation.value = temp_money;
                                    result.first = true;
                                }
                            }
                            common::cls();
                        }
                        break;
                        
                        case 'c':
                        {
                            result.second = true;
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
        }while(!finished);
        return result;
    }
    
    
}