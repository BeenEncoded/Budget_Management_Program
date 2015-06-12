#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>

#include "allocation_distribution_menu.hpp"
#include "data/budget_data.hpp"
#include "utility/scroll_display.hpp"
#include "common/common.hpp"
#include "utility/user_input.hpp"

namespace
{
    template<typename type1, typename type2>
    inline type2 conv(const type1& t)
    {
        type2 t2;
        std::stringstream ss;
        ss<< t;
        ss>> t2;
        return t2;
    }
}

namespace
{
    void create_alloc_display(const std::vector<data::money_alloc_data>&, std::vector<std::string>&);
    std::string allocation_display(const data::money_alloc_data&);
    data::distribution_data::percent_t percent_left(const data::budget_data&);
    void access_alloc_money(data::money_alloc_data&, data::money_t*&);
    bool access_alloc_percentage(data::money_alloc_data&, unsigned int&);
    bool get_user_percent(data::distribution_data::percent_t&);
    
    
    
    /**
     * @brief Constructs a string to display a single allocation for a 
     * window_data_class.  This displays percent alongside the alloc name.
     * @param alloc the allocation.
     * @return a string.
     */
    inline std::string allocation_display(const data::money_alloc_data& alloc)
    {
        using common::fit_str;
        
        constexpr unsigned int name_size{20};
        
        std::string temps{(fit_str(alloc.name, name_size) + 
                std::string((name_size - fit_str(alloc.name, name_size).size()), ' '))};
        if(alloc.meta_data != nullptr)
        {
            if(alloc.meta_data->dist_data.enabled)
            {
                temps += ("%" + std::to_string(alloc.meta_data->dist_data.percent_value));
            }
            else
            {
                temps += "[DISABLED]";
            }
        }
        else
        {
            temps += "[nullptr]";
        }
        return temps;
    }
    
    /**
     * @brief Creates a display for scrollDisplay::window_data_class<data::money_alloc_data>
     * with name and percentage.
     */
    inline void create_alloc_display(const std::vector<data::money_alloc_data>& allocs, 
            std::vector<std::string>& disp)
    {
        disp.erase(disp.begin(), disp.end());
        for(std::vector<data::money_alloc_data>::const_iterator it{allocs.begin()}; it != allocs.end(); ++it)
        {
            disp.push_back(std::move(allocation_display(*it)));
        }
    }
    
    /**
     * @brief Returns the remaining percentage that can be assigned in the budget.
     */
    inline data::distribution_data::percent_t percent_left(const data::budget_data& b)
    {
        data::money_t money{0};
        
        for(std::vector<data::money_alloc_data>::const_iterator it{b.allocs.begin()}; it != b.allocs.end(); ++it)
        {
            if(it->meta_data->dist_data.enabled)
            {
                money += ((b.total_money / 100) * it->meta_data->dist_data.percent_value);
            }
            else
            {
                money += it->value;
            }
        }
        return (100 - (data::distribution_data::percent_t)(((long double)money / b.total_money) * (long double)100));
    }
    
    /**
     * @brief This function is used in the distribution algorithm and modifies
     * its second argument so that it points to a.value.
     */
    inline void access_alloc_money(data::money_alloc_data& a, data::money_t*& m)
    {
        m = (&(a.value));
    }
    
    /**
     * @brief This function is used in the percentage distribution algorithm and
     * modifies its second argument to equal that of a.meta_data->dist_data.percent_value
     * if (a.meta_data != nullptr).
     * @return True if the data::alloc_statistics_data is not null and distribution
     * is enabled for the allocation.
     */
    inline bool access_alloc_percentage(data::money_alloc_data& a, unsigned int& perc)
    {
        perc = 0;
        if(a.meta_data != nullptr)
        {
            perc = a.meta_data->dist_data.percent_value;
            return a.meta_data->dist_data.enabled;
        }
        return false;
    }
    
    /**
     * @brief Allows he user to input a percentage.
     * @param perc The number to store the input into.
     * @return true if the user entered a valid number for a percentage.
     */
    inline bool get_user_percent(data::distribution_data::percent_t& perc)
    {
        bool valid{false}, pressed_enter{false};
        std::string temps{std::move(std::to_string(perc))};
        
        do
        {
            pressed_enter = common::get_user_str(temps);
            valid = (common::str_is_num(temps) && 
                    (conv<std::string, data::distribution_data::percent_t>(temps) <= 100) && 
                    (conv<std::string, data::distribution_data::percent_t>(temps) >= (-100)));
        }while(!valid && pressed_enter);
        if(pressed_enter && valid) perc = conv<std::string, data::distribution_data::percent_t>(temps);
        return (pressed_enter && valid);
    }
    
    
}

namespace submenu
{
    /**
     * @brief Allows the user to distribute money in a budget based on user-defined
     * percentages given to each allocation.
     * @param b The budget.
     * @return true if the user modified and did not cancel.
     */
    bool distribute_by_percent(data::budget_data& b)
    {
        using scrollDisplay::window_data_class;
        using scrollDisplay::display_window;
        using std::cout;
        using std::endl;
        using keyboard::key_code_data;
        
        if(b.allocs.empty()) return false;
        
        /* When we initialize window_data_class, the display creation function is called
         * (in this case, create_alloc_display).  create_alloc_display references
         * alloc_statistics_data in the budget, so that needs to be initialized: */
        data::generate_meta_data(b);
        
        window_data_class<data::money_alloc_data> display{b.allocs, create_alloc_display};
        bool finished{false}, canceled{false}, modified{false};
        key_code_data key;
        
        display.win().window_size() = 8;
        user_input::cl();
        do
        {
            common::cls();
            cout<< endl;
            common::center("Distribution by Percentage:");
            for(unsigned int x{0}; x < 4; ++x) cout<< endl;
            display_window(display);
            cout<< endl<< endl;
            cout<< "Percentage left: %"<< percent_left(b)<< endl<< endl<< endl;
            cout<< " [ENTER] -  Set percentage"<< endl;
            cout<< " [TAB] -  Toggle inclusion"<< endl;
            cout<< endl;
            cout<< " [SPCE] -  Distribute"<< endl;
            cout<< " [BCKSPCE] -  Cancel"<< endl;
            
            key = std::move(user_input::getch_funct());
            
            if(keyboard::is_listed_control(key))
            {
                using keyboard::keys;
                using namespace keyboard::code;
                
                if(key == keys[up::value]) display.win().mv_up();
                else if(key == keys[down::value]) display.win().mv_down();
                else if(key == keys[home::value]) while(display.win().mv_up());
                else if(key == keys[end::value]) while(display.win().mv_down());
                else if(key == keys[backspace::value])
                {
                    if(common::prompt_user("Are you sure you want to cancel percentage distribution?"))
                    {
                        canceled = true;
                        finished = true;
                    }
                }
            }
            else if(!key.control_d.empty())
            {
                switch(std::tolower((char)key.control_d[0]))
                {
                    case '\n':
                    {
                        common::cls();
                        for(unsigned int x{0}; x < 11; ++x) cout<< endl;
                        cout<< "Enter a percentage for \""<< display.selected().name<< "\": %";
                        cout.flush();
                        if(get_user_percent(display.selected().meta_data->dist_data.percent_value)) modified = true;
                    }
                    break;
                    
                    case '\t':
                    {
                        if(display.selected().meta_data != nullptr)
                        {
                            display.selected().meta_data->dist_data.enabled = !display.selected().meta_data->dist_data.enabled;
                        }
                    }
                    break;
                    
                    case ' ':
                    {
                        if(common::prompt_user("Are you sure this is how you want to distribute your money?"))
                        {
                            modified = true;
                            common::distribute_by_percent(b.total_money, b.allocs, access_alloc_money, access_alloc_percentage);
                            finished = true;
                        }
                    }
                    break;
                    
                    default:
                    {
                    }
                    break;
                }
            }
        }while(!finished);
        return (modified && !canceled);
    }
    
    /**
     * @brief Allows the user to choose a method of distribution.
     * @param b The budget.
     * @return true if the budget eas modified.
     */
    bool distribution_selection(data::budget_data& b)
    {
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        if(b.allocs.empty())
        {
            common::cls();
            for(unsigned int x{0}; x < v_center::value; ++x) cout<< endl;
            common::center("There are no allocations!");
            common::wait();
            common::cls();
            return false;
        }
        
        bool finished{false}, modified{false};
        key_code_data key;
        
        user_input::cl();
        do
        {
            common::cls();
            cout<< endl;
            common::center("Choose Distribution type: ");
            for(unsigned int x{0}; x < 4; ++x) cout<< endl;
            cout<< " 1 -  By percent"<< endl;
            cout<< " 2 -  Equally"<< endl;
            cout<< endl;
            cout<< " c -  Cancel"<< endl;
            
            key = std::move(user_input::getch_funct());
            if(!keyboard::is_listed_control(key))
            {
                if(!key.control_d.empty())
                {
                    switch(std::tolower((char)key.control_d[0]))
                    {
                        case '1':
                        {
                            finished = true;
                            modified = distribute_by_percent(b);
                        }
                        break;
                        
                        case '2':
                        {
                            finished = true;
                            common::distribute_equally(b.total_money, b.allocs, access_alloc_money);
                            modified = true;
                        }
                        break;
                        
                        case 'c':
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
        }while(!finished);
        return modified;
    }
    
    
}