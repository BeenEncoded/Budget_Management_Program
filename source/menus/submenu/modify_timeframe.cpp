#include <utility>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <exception>
#include <sstream>
#include <string>
#include <stdexcept>

#include "modify_timeframe.hpp"
#include "utility/stream_operations.hpp"
#include "data/budget_data.hpp"
#include "utility/time_frame.hpp"
#include "common/global/program_data.hpp"
#include "common/common.hpp"
#include "utility/user_input.hpp"
#include "utility/scroll_display.hpp"
#include "utility/filesystem.hpp"

namespace
{
    void unit_display(const std::vector<tdata::time_interval_type::unit_t>&, std::vector<std::string>&);
    std::vector<tdata::time_interval_type::unit_t> time_units();
    std::string interval_display(const tdata::time_interval_type&);
    
    
    
    template<typename type>
    type to_other(const std::string& s)
    {
        std::stringstream ss;
        ss<< s;
        type t;
        ss>> t;
        return t;
    }
    
    /**
     * @brief Allows the user to select an element from a list using a scroll
     * display.  Returns true if the user selected an element, false if the user canceled.
     */
    template<typename type>
    bool user_select_element(scrollDisplay::window_data_class<type>& disp, type& val)
    {
        if(disp.gdata()->empty()) return false;
        if(disp.gdata()->size() == 1) return false; //there's no point...
        
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        //select the value that is currently set:
        while(disp.win().mv_up());
        for(unsigned int x{0}; ((x < disp.gdata()->size()) && (disp.selected() != val)); ++x) disp.win().mv_down();
        if(disp.selected() != val) while(disp.win().mv_up());
        
        bool finished{false}, canceled{false};
        key_code_data key;
        const type original_value{val};
        
        user_input::cl();
        disp.win().window_size() = 10;
        do
        {
            common::cls();
            common::center("Select:");
            for(unsigned int x{0}; x < 2; ++x) cout<< endl;
            scrollDisplay::display_window<type>(disp, std::pair<char, char>{'>', '<'});
            cout<< endl;
            cout<< " [ENTR] -  Select"<< endl;
            cout<< " C -  Cancel"<< endl;
            
            key = std::move(user_input::getch_funct());
            if(keyboard::is_listed_control(key))
            {
                using keyboard::keys;
                using namespace keyboard::code;
                
                if(key == keys[up::value]) disp.win().mv_up();
                else if(key == keys[down::value]) disp.win().mv_down();
                else if(key == keys[pgup::value]) disp.win().pg_up();
                else if(key == keys[pgdown::value]) disp.win().pg_down();
                else if(key == keys[home::value]) while(disp.win().mv_up());
                else if(key == keys[end::value]) while(disp.win().mv_down());
            }
            else if(!key.control_d.empty())
            {
                switch(std::tolower((char)key.control_d[0]))
                {
                    case '\n':
                    {
                        val = disp.selected();
                        finished = true;
                    }
                    break;
                    
                    case 'c':
                    {
                        finished = true;
                        canceled = true;
                    }
                    break;
                    
                    default:
                    {
                    }
                    break;
                }
            }
        }while(!finished);
        if(canceled) val = original_value;
        return !canceled;
    }
    
    inline void unit_display(const std::vector<tdata::time_interval_type::unit_t>& units, std::vector<std::string>& display)
    {
        std::vector<std::pair<tdata::time_interval_type::unit_t, std::string> > unit_list{tdata::time_interval_type::unit_names()};
        
        display.erase(display.begin(), display.end());
        for(unsigned int x{0}; x < units.size(); ++x)
        {
            display.push_back(unit_list[x].second);
        }
    }
    
    inline std::vector<tdata::time_interval_type::unit_t> time_units()
    {
        std::vector<tdata::time_interval_type::unit_t> u;
        std::vector<std::pair<tdata::time_interval_type::unit_t, std::string> > units{tdata::time_interval_type::unit_names()};
        
        for(unsigned int x{0}; x < units.size(); ++x)
        {
            u.push_back(units[x].first);
        }
        return u;
    }
    
    inline std::string interval_display(const tdata::time_interval_type& t)
    {
        return (std::to_string(t.count) + " " + ((t.count != 1) ? (t.unit_name() + "s") : t.unit_name()));
    }
    
    
}

namespace menu
{
    /**
     * @brief Allows the user to modify a time_interval_type.
     * @return a std::pair<bool, bool>:
     * first = whether the user modified anything
     * second = whether the user canceled the operation.
     */
    std::pair<bool, bool> modify_interval(tdata::time_interval_type& i)
    {
        using std::cout;
        using std::endl;
        using keyboard::key_code_data;
        
        bool finished{false};
        std::pair<bool, bool> result{false, false};
        key_code_data key;
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Modify the length of time for the budget:");
            for(unsigned int x{0}; x < 2; ++x) cout<< endl;
            cout<< " 1 -  Count: "<< i.count<< endl;
            cout<< " 2 -   Unit: "<< i.unit_name()<< endl;
            
            cout<< " c -  Cancel"<< endl;
            cout<< " [BKSPC] -  Done"<< endl;
            
            key = std::move(user_input::getch_funct());
            if(keyboard::is_listed_control(key))
            {
                using keyboard::keys;
                using namespace keyboard::code;
                
                if(key == keys[backspace::value]) finished = true;
            }
            else if(!key.control_d.empty())
            {
                switch(std::tolower((char)key.control_d[0]))
                {
                    case '1':
                    {
                        common::cls();
                        for(unsigned int x{0}; x < v_center::value; ++x) cout<< endl;
                        cout<< "Unit = "<< i.unit_name()<< endl;
                        cout<< "Enter a count: ";
                        cout.flush();
                        
                        bool value_entered{false};
                        std::string temps{std::to_string(i.count)};
                        do
                        {
                            value_entered = common::get_user_str(temps);
                        }while(value_entered && !common::str_is_num(temps));
                        if(value_entered)
                        {
                            i.count = std::move(to_other<decltype(i.count)>(temps));
                            result.first = true;
                        }
                        temps.erase();
                    }
                    break;
                    
                    case '2':
                    {
                        using scrollDisplay::window_data_class;
                        
                        std::vector<decltype(i.u)> units{time_units()};
                        window_data_class<decltype(i.u)> temp_window{units, unit_display};
                        
                        if(user_select_element<decltype(i.u)>(temp_window, i.u)) result.first = true;
                    }
                    break;
                    
                    case 'c':
                    {
                        finished = true;
                        result.second = true;
                    }
                    break;
                    
                    default:
                    {
                    }
                    break;
                }
            }
        }while(!finished);
        return result;
    }
    
    /**
     * @brief Allows the user to modify the timeframe of a budget.
     * @param pdat program data.  Used for making sure that the timeframe doesn't
     * conflict with another budget's timeframe.
     * @param bud The budget to which the timeframe belongs.
     * @return std::pair<bool, bool>: 
     * first -> whather the timeframe was modified
     * second -> whether the user canceled all modification.
     */
    std::pair<bool, bool> modify_timeframe(tdata::timeframe_class& t)
    {
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        bool finished{false};
        std::pair<bool, bool> result{false, false};
        key_code_data key;
        
        user_input::cl();
        do
        {
            common::cls();
            cout<< endl;
            common::center("Edit Timeframe:");
            for(unsigned int x{0}; x < 2; ++x) cout<< endl;
            cout<< " 1 -  Edit date frame begins:"<< common::date_disp(t.beg)<< endl;
            cout<< " 2 -  Edit length of time the budget spans: "<< interval_display(t.cinterval())<< endl;
            cout<< endl;
            cout<< " c -  Cancel"<< endl;
            cout<< " [BCKSPC] -  Done"<< endl;
            
            key = std::move(user_input::getch_funct());
            if(keyboard::is_listed_control(key))
            {
                using keyboard::keys;
                using namespace keyboard::code;
                
                if(key == keys[backspace::value]) finished = true;
            }
            else if(!key.control_d.empty())
            {
                switch(std::tolower((char)key.control_d[0]))
                {
                    case '1':
                    {
                        if(common::user_choose_date(t.beg)) result.first = true;
                    }
                    break;
                    
                    case '2':
                    {
                        tdata::time_interval_type temp_interval{t.cinterval()};
                        std::pair<bool, bool> temp_res{modify_interval(temp_interval)};
                        
                        //if modified and not canceled:
                        if(temp_res.first && !temp_res.second)
                        {
                            t = std::move(tdata::timeframe_class{t.beg, temp_interval});
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
        }while(!finished);
        return result;
    }
    
    
}