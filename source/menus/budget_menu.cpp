#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <limits>

#include "budget_menu.hpp"
#include "utility/scroll_display.hpp"
#include "utility/user_input.hpp"
#include "common/common.hpp"
#include "data/budget_data.hpp"
#include "utility/scroll_display.hpp"
#include "utility/filesystem.hpp"
#include "utility/stream_operations.hpp"
#include "submenu/move_element.hpp"
#include "submenu/allocation_distribution_menu.hpp"
#include "common/global/global_defines.hpp"
#include "submenu/modify_timeframe.hpp"
#include "utility/time_frame.hpp"


/* budget_statistics_data: */
namespace
{
    typedef struct budget_statistics_data budget_statistics_data;
    
    
    /**
     * @struct budget_statistics_data
     * @author Jonathan Whitlock 
     * @date 06/04/2015
     * @file budget_menu.cpp
     * @brief Just trivializes statistics for a budget.  Use it or don't.  Doesn't matter.
     * It mainly makes it easier to calculate, display, and maintain statistics
     * for the budget.
     */
    typedef struct budget_statistics_data
    {
        explicit budget_statistics_data();
        explicit budget_statistics_data(const data::budget_data&);
        budget_statistics_data(const budget_statistics_data&);
        budget_statistics_data(budget_statistics_data&&) noexcept;
        
        ~budget_statistics_data();
        
        budget_statistics_data& operator=(const budget_statistics_data&);
        budget_statistics_data& operator=(budget_statistics_data&&) noexcept;
        bool operator==(const budget_statistics_data&) const;
        bool operator!=(const budget_statistics_data&) const;
        
        const data::budget_data *budget;
        data::money_t money_allocated, money_unallocated;
    } budget_statistics_data;
    
    
    budget_statistics_data::budget_statistics_data() : 
            budget{nullptr},
            money_allocated{0},
            money_unallocated{0}
    {
    }
    
    budget_statistics_data::budget_statistics_data(const data::budget_data& b) : 
            budget{&b},
            money_allocated{0},
            money_unallocated{0}
    {
        for(std::vector<data::money_alloc_data>::const_iterator it{b.allocs.begin()}; it != b.allocs.end(); ++it)
        {
            this->money_allocated += it->value;
        }
        this->money_unallocated = (this->budget->total_money - this->money_allocated);
    }
    
    budget_statistics_data::budget_statistics_data(const budget_statistics_data& s) : 
            budget{s.budget},
            money_allocated{s.money_allocated},
            money_unallocated{s.money_unallocated}
    {
    }
    
    budget_statistics_data::budget_statistics_data(budget_statistics_data&& s) noexcept : 
            budget{std::move(s.budget)},
            money_allocated{std::move(s.money_allocated)},
            money_unallocated{std::move(s.money_unallocated)}
    {
    }
    
    budget_statistics_data::~budget_statistics_data()
    {
        this->budget = nullptr;
    }
    
    budget_statistics_data& budget_statistics_data::operator=(const budget_statistics_data& s)
    {
        if(this != &s)
        {
            this->budget = s.budget;
            this->money_allocated = s.money_allocated;
            this->money_unallocated = s.money_unallocated;
        }
        return *this;
    }
    
    budget_statistics_data& budget_statistics_data::operator=(budget_statistics_data&& s) noexcept
    {
        if(this != &s)
        {
            this->budget = std::move(s.budget);
            this->money_allocated = std::move(s.money_allocated);
            this->money_unallocated = std::move(s.money_unallocated);
        }
        return *this;
    }
    
    bool budget_statistics_data::operator==(const budget_statistics_data& s) const
    {
        return (((*(this->budget)) == (*(s.budget))) && 
                    (this->money_allocated == s.money_allocated) && 
                    (this->money_unallocated == s.money_unallocated));
    }
    
    bool budget_statistics_data::operator!=(const budget_statistics_data& s) const
    {
        return (((*(this->budget)) != (*(s.budget))) || 
                    (this->money_allocated != s.money_allocated) || 
                    (this->money_unallocated != s.money_unallocated));
    }
    
    
}

/* Local functions that don't really belong anywhere but here.  There are also lesser 
 * functions used directly in menus that are within this namespace as well:*/
namespace
{
    data::budget_data load_basic_info(const std::string&);
    void display_error(const std::string&);
    std::vector<data::budget_data> load_basic_info_all(global::program_data&);
    budget_statistics_data apply_allocation(data::budget_data&, const data::money_alloc_data&);
    bool sort_compare_budgets(const std::string&, const std::string&);
    void convert_to_money(std::string&);
    std::string allocation_display(const data::money_alloc_data&);
    std::string money_display(const data::money_t&);
    bool timeframe_conflict(const data::budget_data&, global::program_data&);
    
    
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
            std::ifstream in{path.c_str(), std::ios::in};
            if(in.good()) in_mem(in, tempbud.total_money);
            if(in.good()) in_mem(in, tempbud.id);
            if(in.good()) in>> tempbud.timeframe;
            if(in.is_open()) in.close();
        }
        return tempbud;
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
     * @brief Takes a string representing a (long double), and
     * formats it so that it represents a data::money_t.  This was
     * created because reading (long double) from a std::stringstream
     * resulted in inaccuracies that modified the input of the user
     * to an unacceptable degree.
     * @param s The string to format.
     */
    inline void convert_to_money(std::string& s)
    {
        std::size_t pos{s.find('.')};
        if(pos != std::string::npos)
        {
            if((s.size() - (pos + 1)) > 2)
            {
                s.insert((s.begin() + (pos + 3)), '.');
                s.erase((s.begin() + pos));
            }
            else if((s.size() - (pos + 1)) == 2)
            {
                s.erase((s.begin() + pos));
            }
            else
            {
                s.erase((s.begin() + pos));
                s += "0";
            }
        }
        else
        {
            s += "00";
        }
    }
    
    /**
     * @brief Applies an allocation to a budget for the purpose of calculating it's
     * statistics.  The budget is not changed, and all modification is temporary.
     * @param budget The budget to use.
     * @param allocation The allocation data.
     * @return budget_statistics_data on a budget that has the allocation applied.
     */
    inline budget_statistics_data apply_allocation(data::budget_data& budget, const data::money_alloc_data& allocation)
    {
        budget_statistics_data stats;
        data::money_alloc_data original_alloc;
        bool found{false};
        
        for(std::vector<data::money_alloc_data>::iterator it{budget.allocs.begin()}; ((it != budget.allocs.end()) && !found); ++it)
        {
            if((allocation.id == it->id) && !found)
            {
                found = true;
                original_alloc = std::move(*it);
                (*it) = allocation;
                
                stats = std::move(budget_statistics_data{budget});
                (*it) = std::move(original_alloc);
            }
        }
        if(!found)
        {
            budget.allocs.push_back(allocation);
            stats = std::move(budget_statistics_data{budget});
            budget.allocs.pop_back();
        }
        return stats;
    }
    
    /**
     * @brief Used to sort budgets for budget_list_menu.
     * @return true if
     */
    inline bool sort_compare_budgets(const std::string& b1, const std::string& b2)
    {
        return (load_basic_info(b1).timeframe.beg >= load_basic_info(b2).timeframe.beg);
    }
    
    /**
     * @brief creates a string representation of a money_alloc_data.
     */
    inline std::string allocation_display(const data::money_alloc_data& a)
    {
        using common::fit_str;
        
        constexpr unsigned int name_size{20};
        
        std::string temps{(fit_str(a.name, name_size) + std::string((name_size - fit_str(a.name, name_size).size()), ' ') + 
                std::string(4, ' ') + money_display(a.value)) + std::string((10 - money_display(a.value).size()), ' ') + 
                "Balance: "};
        if(a.meta_data != nullptr)
        {
            temps += money_display(a.meta_data->balance);
        }
        else
        {
            temps += "[Error dereferencing data::money_alloc_data::meta_data::balance]";
        }
        return temps;
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
    
    inline bool timeframe_conflict(const data::budget_data& b, global::program_data& pdata)
    {
        data::budget_data tempbud;
        
        pdata.budget_files = std::move(global::budget_paths(pdata.budget_folder));
        for(std::vector<std::string>::iterator it{pdata.budget_files.begin()}; it != pdata.budget_files.end();)
        {
            if(!fsys::is_file(*it)) //just a redundancy
            {
                it = pdata.budget_files.erase(it);
                continue;
            }
            tempbud = std::move(load_basic_info(*it));
            if(tempbud.id != b.id)
            {
                if(tempbud.timeframe.overlaps(b.timeframe)) return true;
            }
            ++it;
        }
        return false;
    }
    
    
}

//Primary functions that are directly used in the menus:
namespace
{
    bool user_get_money_value(data::money_t&);
    std::string budget_statistic_display(const data::budget_data&);
    bool call_create_budget(global::program_data&);
    void money_alloc_list_display(const std::vector<data::money_alloc_data>&, std::vector<std::string>&);
    bool call_mod_budget(global::program_data&, const std::string&);
    bool usr_delete_file(const std::string&);
    void create_display(const std::vector<std::string>&, std::vector<std::string>&);
    bool spec_budget_timeframe(global::program_data&, data::budget_data&);
    
    
    /**
     * @brief Gets a monetary value from the user.
     * @param m money to modify
     * @return true if the user entered a new value.
     */
    inline bool user_get_money_value(data::money_t& m)
    {
        //todo add ability to recognize and parse simple equations
        std::string temps{std::move(std::to_string(((long double)m / (long double)100)))};
        bool modified{false}, is_valid{false};
        
        do
        {
            modified = common::get_user_str(temps);
            is_valid = common::str_is_num(temps);
        }while(modified && !is_valid);
        if(modified && is_valid)
        {
            convert_to_money(temps);
            std::stringstream ss;
            ss<< temps;
            ss>> m;
        }
        return (modified && is_valid);
    }
    
    /**
     * @brief Creates a std::string that contains a display for budget statistics.
     * @param budget Budget to display stats for.
     * @return a string.
     */
    inline std::string budget_statistic_display(const data::budget_data& budget)
    {
        std::string temps{"Budget for date: " + common::date_disp(budget.timeframe.beg) + "\n\n"};
        budget_statistics_data stats{budget};
        
        temps += ("Total money in the budget: " + money_display(budget.total_money) + "\n");
        temps += ("    Total money allocated: " + money_display(stats.money_allocated) + "\n\n");
        temps += ("               Money left: " + money_display(stats.money_unallocated));
        return temps;
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
            if(fsys::is_file(paths[x]).value)
            {
                tempbud = std::move(load_basic_info(paths[x]));
                display.push_back(std::move((common::date_disp(tempbud.timeframe.beg) + "   (" + 
                        money_display(tempbud.total_money) + ")")));
            }
            else
            {
                display.push_back("ERROR: File doesn't exist!");
            }
        }
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
     * @brief Calls modify_budget, but also handles the loading
     * and saving of the budget, as well as displaying any errors that occur.
     * This essentially seperates the loading/saving code from the menus.  Menus
    budget_statistic_display * should not save or load any data.
     * @param path the path chosen.
     * @return true if the budget was modified.
     */
    inline bool call_mod_budget(global::program_data& pdat, const std::string& path)
    {
        bool success(true);
        data::budget_data tempbud;
        
        if(common::load_from_file(path, tempbud))
        {
            std::pair<bool, bool> tempres{menu::modify_budget(pdat, tempbud)};
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
     * @brief Allows the user to choose a timeframe for a budget.
     * @return true if the user entered a valid timeframe that doesn't conflict with
     * any existing budget's.
     */
    inline bool spec_budget_timeframe(global::program_data& pdata, data::budget_data& bud)
    {
        std::pair<bool, bool> res{false, false};
        bool valid{false};
        tdata::timeframe_class frame{bud.timeframe};
        data::budget_data temp_budget;
        
        do
        {
            res = std::move(menu::modify_timeframe(frame));
            pdata.budget_files = std::move(global::budget_paths(pdata.budget_folder));
            valid = (res.first && !res.second);
            if(valid) //to keep the "overlaps" error from showing if user cancels
            {
                for(std::vector<std::string>::const_iterator it{pdata.budget_files.begin()}; ((it != pdata.budget_files.end()) && valid); ++it)
                {
                    temp_budget = std::move(load_basic_info(*it));
                    if(bud.id != temp_budget.id)
                    {
                        valid = !temp_budget.timeframe.overlaps(frame);
                    }
                }
                if(!valid)
                {
                    using std::cout;
                    using std::endl;
                    
                    common::cls();
                    for(unsigned int x{0}; x < v_center::value; ++x) cout<< endl;
                    common::center("Another budget's timeframe overlaps with the one you chose.  You \
    must choose another!");
                    common::wait();
                    common::cls();
                }
            }
        }while(!valid && !res.second);
        if(valid && res.first && !res.second) bud.timeframe = std::move(frame);
        return (valid && res.first && !res.second);
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
        tempb.timeframe.beg = tdata::current_time();
        if(!spec_budget_timeframe(pdata, tempb)) return budget_created;
        temp_result = std::move(menu::modify_budget(pdata, tempb));
        if(temp_result.first && !temp_result.second) //modified, but not canceled
        {
            if(!common::save_to_file(data::budget_path(pdata, tempb), tempb))
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
     * @brief Allows the user to modify the length of time a budget remains active.
     * @return True if the length was modified.
     */
    inline bool modify_budget_length(global::program_data& pdata, data::budget_data& b)
    {
        using std::cout;
        using std::endl;
        
        tdata::time_interval_type temp_interval{b.timeframe.cinterval()};
        std::pair<bool, bool> tempres{std::move(menu::modify_interval(temp_interval))};
        
        if(tempres.first && !tempres.second)
        {
            data::budget_data temp_bud{b};
            temp_bud.timeframe = std::move(tdata::timeframe_class{temp_bud.timeframe.beg, temp_interval});
            
            while(tempres.first && !tempres.second && 
                    timeframe_conflict(temp_bud, pdata))
            {
                common::cls();
                for(unsigned int x{0}; x < v_center::value; ++x) cout<< endl;
                common::center("The chosen length of time overlaps with another timeframe!");
                common::wait();
                common::cls();
                temp_interval = b.timeframe.cinterval();
                tempres = std::move(menu::modify_interval(temp_interval));
                temp_bud.timeframe = std::move(tdata::timeframe_class{temp_bud.timeframe.beg, temp_interval});
            }
            if(tempres.first && !tempres.second)
            {
                b.timeframe = std::move(tdata::timeframe_class{b.timeframe.beg, temp_interval});
                return true;
            }
        }
        return false;
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
        std::sort(pdat.budget_files.begin(), pdat.budget_files.end(), sort_compare_budgets);
        do
        {
            common::cls();
            cout<< endl;
            common::center("Budgets: ");
            cout<< endl;
            display_window(scroll_window);
            cout<< endl;
            cout<< "\'a\' -  Add new budget"<< endl;
            cout<< "\'q\' -  Quit";
            cout.flush();
            
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
                        if(!pdat.budget_files.empty())
                        {
                            if(common::prompt_user("Are you sure you want to delete the \
budget for " + common::date_disp(load_basic_info(scroll_window.cselected()).timeframe.beg) + "?  This \
is permanent!"))
                            {
                                if(usr_delete_file(scroll_window.cselected()))
                                {
                                    scroll_window.remove_selected();
                                }
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
                                if(!pdat.budget_files.empty())
                                {
                                    call_mod_budget(pdat, scroll_window.cselected());
                                    pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
                                    std::sort(pdat.budget_files.begin(), pdat.budget_files.end(), sort_compare_budgets);
                                }
                            }
                            break;
                            
                            case 'a':
                            {
                                call_create_budget(pdat);
                                pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
                                std::sort(pdat.budget_files.begin(), pdat.budget_files.end(), sort_compare_budgets);
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
    std::pair<bool, bool> modify_budget(global::program_data& pdat __attribute__((unused)), data::budget_data& b)
    {
        //todo add a budget report that shows all allocations, balances, and subtotals that can be easily shared and looks nice.
        using scrollDisplay::window_data_class;
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        std::pair<bool, bool> result{false, false};
        data::budget_data original_budget{b};
        bool finished{false};
        window_data_class<data::money_alloc_data> scroll_display{b.allocs, money_alloc_list_display};
        key_code_data key;
        
        scroll_display.win().window_size() = 6;
        user_input::cl();
        do
        {
            common::cls();
            cout<< "Today is "<< common::date_disp(tdata::time_class{tdata::current_time()})<< endl;
            cout<< endl;
            common::center("Budget starting " + common::date_disp(b.timeframe.beg));
            cout<< endl;
            cout<< "Budget ends "<< common::date_disp(b.timeframe.cinterval().add_to(b.timeframe.beg))<< endl;
            
            data::generate_meta_data(b);
            scrollDisplay::display_window(scroll_display);
            data::delete_meta_data(b);
            
            cout<< endl;
            cout<< "Total money in this budget: "<< money_display(b.total_money)<< endl;
            cout<< endl;
            cout<< " m -------> Modify total money to the budget"<< endl;
            cout<< " a -------> Add new allocation"<< endl;
            cout<< " d -------> Distribute money"<< endl;
            cout<< " [SPCE] --> Move Selected"<< endl;
            cout<< " s -------> Statistics"<< endl;
            cout<< " L -------> Length of time the budgets is active"<< endl;
            cout<< endl;
            cout<< " [BCKSPC] -  Done"<< endl;
            cout<< " c -  -----  Cancel";
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
                                std::pair<bool, bool> temp_result{modify_allocation(b, temp_allocation)};
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
                            data::money_alloc_data new_allocation{"", (data::money_t)0};
                            new_allocation.id = data::new_alloc_id(b.allocs);
                            std::pair<bool, bool> temp_result{modify_allocation(b, new_allocation)};
                            if(temp_result.first && !temp_result.second)
                            {
                                b.allocs.push_back(std::move(new_allocation));
                                result.first = true;
                            }
                        }
                        break;
                        
                        case 'l':
                        {
                            if(modify_budget_length(pdat, b)) result.first = true;
                        }
                        break;
                        
                        case 's':
                        {
                            common::cls();
                            cout<< budget_statistic_display(b)<< endl<< endl;
                            common::wait();
                            common::cls();
                        }
                        break;
                        
                        case 'c':
                        {
                            result.second = true;
                            finished = true;
                        }
                        break;
                        
                        case 'm':
                        {
                            common::cls();
                            for(unsigned int x{0}; x < v_center::value; ++x) cout<< endl;
                            cout<< "Current money budgeted: "<< money_display(b.total_money)<< endl;
                            cout<< "Money left in budget: "<< money_display(budget_statistics_data{b}.money_unallocated)<< endl;
                            cout<< endl<< endl;
                            cout<< "Money that would zero budget: "<< money_display(budget_statistics_data{b}.money_allocated)<< endl;
                            cout<< endl;
                            cout<< "Enter the total money you have for this budget: $";
                            cout.flush();
                            if(user_get_money_value(b.total_money))
                            {
                                result.first = true;
                            }
                        }
                        break;
                        
                        case 'd':
                        {
                            if(submenu::distribution_selection(b)) result.first = true;
                            data::delete_meta_data(b);
                        }
                        break;
                        
                        case ' ':
                        {
                            data::generate_meta_data(b);
                            //note this is how I'm implementing priority: based on order of the elements.
                            if(b.allocs.size() > 1)
                            {
                                std::function<void()> update_disp{[&b]()->void{data::generate_meta_data(b);}};
                                if(submenu::move_vector_element(scroll_display, b.allocs, &update_disp))
                                {
                                    result.first = true;
                                }
                            }
                            data::delete_meta_data(b);
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
        if(result.second && result.first) b = std::move(original_budget);
        return result;
    }
    
    /**
     * @brief Allows the user to modify a budget allocation.
     * @param allocation The allocation to modify.
     * @param budget The budget referenced.  This is mainly used for 
     * making calculations about how much money is left in the budget.  The budget
     * is not modified.
     * @return a std::pair<bool, bool>:
     * first = whether the allocation was modified
     * second = whether the user canceled modification of the allocation.
     */
    std::pair<bool, bool> modify_allocation(data::budget_data& budget, data::money_alloc_data& allocation)
    {
        using std::cout;
        using std::endl;
        
        std::pair<bool, bool> result{false, false};
        bool finished{false};
        keyboard::key_code_data key;
        
        do
        {
            common::cls();
            cout<< "Money that can be allocated: "<< money_display(apply_allocation(budget, allocation).money_unallocated)<< endl;
            cout<< endl;
            common::center("Modify Budget Allocation");
            for(unsigned int x{0}; x < 3; ++x) cout<< endl;
            cout<< " 1 -  Name: \""<< allocation.name<< "\""<< endl;
            cout<< " 2 -  Value: "<< money_display(allocation.value)<< endl;
            cout<< endl;
            cout<< " z -  Zero out"<< endl;
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
                            std::string temps{((allocation.name == data::money_alloc_data{}.name) ? "" : allocation.name)};
                            common::cls();
                            cout<< "[ESC]: cancel"<< endl;
                            for(unsigned int x{0}; x < 11; ++x) cout<< endl;
                            cout<< "Enter a name: ";
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
                        
                        case 'z':
                        {
                            allocation.value += apply_allocation(budget, allocation).money_unallocated;
                            result.first = true;
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