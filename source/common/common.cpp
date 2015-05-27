#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <cstring>
#include <cmath>

#include "utility/ansi.hpp"
#include "global/global_defines.hpp"
#include "common/common.hpp"
#include "common/global/program_data.hpp"
#include "utility/filesystem.hpp"
#include "utility/time_class.hpp"
#include "data/budget_data.hpp"
#include "utility/user_input.hpp"
#include "utility/time_class.hpp"

namespace
{
    std::string parent_path(const std::string&);
    std::string date_selection_display(
            const tdata::time_class&,
            const unsigned short&,
            const std::pair<char, char>&);
    
    void date_selection_add(tdata::time_class&, const unsigned short&, const long&);
    
    
    
    inline std::string parent_path(const std::string& s)
    {
        std::string temps(s);
        std::string::size_type pos(temps.rfind('/'));
        
        if((pos != std::string::npos) && (pos > 0))
        {
            temps.erase((temps.begin() + pos), temps.end());
        }
        else temps.erase();
        return temps;
    }
    
    /**
     * @brief Creates a string that represents a date, but adds brackets
     * around a specified selection.
     * @param sel
     * 0 = month
     * 1 = day
     * 2 = year
     * @return " month  [day]  year "
     */
    inline std::string date_selection_display(
            const tdata::time_class& t,
            const unsigned short& sel,
            const std::pair<char, char>& brack)
    {
        std::string temps;
        std::pair<char, char> brackets[3]{
            ((0 == sel) ? brack : std::pair<char, char>{' ', ' '}),
            ((1 == sel) ? brack : std::pair<char, char>{' ', ' '}),
            ((2 == sel) ? brack : std::pair<char, char>{' ', ' '})
        };
        
        temps = (brackets[0].first + t.month_name() + brackets[0].second);
        temps += (brackets[1].first + std::to_string(t.mday()) + brackets[1].second);
        temps += (brackets[2].first + std::to_string(t.gyear()) + brackets[2].second);
        return temps;
    }
    
    /**
     * @brief Adds 'num' units to the selected date unit.
     * @param sel Selected:
     * 0 month
     * 1 day
     * 2 year
     * @param num number to increment (or decrement) by.
     */
    inline void date_selection_add(tdata::time_class& t, const unsigned short& sel, const long& n)
    {
        if(n == 0) return;
        
        long num{((n < 0) ? (n * (-1)) : n)};
        bool subtract{(n < 0)};
        switch(sel)
        {
            case 0:
            {
                if(!subtract)
                {
                    int temp_month;
                    for(long x{0}; x < num; ++x)
                    {
                        temp_month = t.month();
                        for(unsigned int y{0}; ((y < 33) && (temp_month == t.month())); ++y)
                        {
                            t += tdata::t_const::day;
                        }
                    }
                }
                else
                {
                    int temp_month;
                    for(long x{0}; x < num; ++x)
                    {
                        temp_month = t.month();
                        for(unsigned int y{0}; ((y < 33) && (temp_month == t.month())); ++y)
                        {
                            t -= tdata::t_const::day;
                        }
                    }
                }
            }
            break;
            
            case 1:
            {
                if(!subtract)
                {
                    for(long x{0}; x < num; ++x) t += tdata::t_const::day;
                }
                else
                {
                    for(long x{0}; x < num; ++x) t -= tdata::t_const::day;
                }
            }
            break;
            
            case 2:
            {
                if(!subtract)
                {
                    for(long x{0}; x < num; ++x) t.syear((t.gyear() + 1));
                }
                else
                {
                    for(long x{0}; x < num; ++x) t.syear((t.gyear() - 1));
                }
            }
            break;
            
            default:
            {
                ethrow(std::string("Error in \"") + "inline void date_selection\
_add(tdata::time_class& t, const unsigned short& sel, const long& num): \
Selection invalid!");
            }
            break;
        }
    }
    
    
}

//result_data member functions:
namespace common
{
    /* The definition of result_data has to go before the rest of the functions'
     * definitions because this object must be instantiated before it's use in 
     * any of the functions. */
    
    
    template<typename type>
    result_data<type>::result_data() noexcept : 
            success{false},
            data{},
            message{}
    {
    }
    
    template<typename type>
    result_data<type>::result_data(const result_data<type>& d) noexcept : 
            success{d.success},
            data{d.data},
            message{d.message}
    {
    }
    
    template<typename type>
    result_data<type>::result_data(result_data<type>&& d) noexcept : 
            success{std::move(d.success)},
            data{std::move(d.data)},
            message{std::move(d.message)}
    {
    }
    
    template<typename type>
    result_data<type>::~result_data()
    {
    }
    
    template<typename type>
    result_data<type>& result_data<type>::operator=(const result_data<type>& r)
    {
        if(this != &r)
        {
            this->success = r.success;
            this->data = r.data;
            this->message = r.message;
        }
        return *this;
    }
    
    template<typename type>
    result_data<type>& result_data<type>::operator=(result_data<type>&& r) noexcept
    {
        this->success = std::move(r.success);
        this->data = std::move(r.data);
        this->message = std::move(r.message);
        return *this;
    }
    
    template<typename type>
    bool result_data<type>::operator!() const
    {
        return !this->success;
    }
    
    template<typename type>
    bool result_data<type>::operator==(const result_data<type>& t) const
    {
        return ((this->success == t.success) && 
                (this->data == t.data) && 
                (this->message == t.message));
    }
    
    template<typename type>
    bool result_data<type>::operator!=(const result_data<type>& t) const
    {
        return ((this->success != t.success) || 
                (this->data != t.data) || 
                (this->message != t.message));
    }
    
    template struct result_data<void*>;
    template struct result_data<global::program_data>;
    template struct result_data<std::pair<tdata::time_class, data::money_t> >;
    template struct result_data<bool>;
}

namespace common
{
    /**
     * @brief Loads an object from a file.  Type requirements include an overloaded 
     * operator>>
     * @param path The full path to the file.
     * @param t The object to read the data into.
     * @return true upon graceful execution.
     */
    template<typename type>
    bool load_from_file(const std::string& path, type& t)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        
        t = type();
        if(is_symlink(path).value || !is_file(path).value) return false;
        
        std::ifstream in(path, std::ios::binary);
        bool success(true);
        
        in.peek();
        if(in.good())
        {
            in>> t;
            success = !in.fail();
        }
        else success = false;
        if(in.is_open()) in.close();
        return success;
    }
    
    template bool load_from_file<data::budget_data>(const std::string&, data::budget_data&);
    
    /**
     * @brief Saves an object to a file.  Type requirements include an overloaded
     * operator<<
     * @param path The full path to the file.  WARNING:  If the parent folders
     * don't already exist, then they will be created.
     * @param t The object to write.
     * @return true upon graceful execution.
     */
    template<typename type>
    bool save_to_file(const std::string& path, const type& t)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::create_folder;
        
        {
            std::string f(parent_path(path));
            if(!create_folder(f).value)
            {
                if(!is_folder(f).value) return false;
            }
        }
        
        std::ofstream out(path, std::ios::binary);
        if(out.good()) out<< t;
        bool success = !out.fail();
        if(out.is_open()) out.close();
        return success;
    }
    
    template bool save_to_file<data::budget_data>(const std::string&, const data::budget_data&);
    
    /**
     * @brief returns a specified digit within a number.
     * @param num the number to perform the operation on.
     * @param place the place you want to get.  For example,
     * if you want the digit at the 10's place, then you
     * would pass 2.
     * @return A single-digit number that represents the
     * digit at the specified place.
     */
    int digit(const unsigned int& num, const unsigned short& place)
    {
        return (((num % (unsigned int)pow(10, place)) - (num % (unsigned int)pow(10, (place - 1)))) / (10 ^ (place - 1)));
    }
    
    /**
     * @brief Retrieves the number of digits in a number.
     * @param num the integer to operate on
     * @return the number of digits in num.
     */
    int digit_count(const unsigned int& num)
    {
        int count(0);
        unsigned int temp(num);
        while(temp >= 10)
        {
            temp /= 10;
            ++count;
        }
        return ++count;
    }
    
    bool is_letter(const char& ch)
    {
        for(unsigned char x('a'), y('A'); ((x <= 'z') && (y <= 'Z')); ++x, ++y)
        {
            if((ch == (signed)x) || (ch == (signed)y))
            {
                return true;
            }
        }
        return false;
    }
    
    bool is_special(const char& ch)
    {
        return (std::string(SPECIALS).find(ch) != std::string::npos);
    }
    
    bool is_number(const char& ch)
    {
        return (std::string(NUMBERS).find(ch) != std::string::npos);
    }
    
    bool is_char(const char& ch)
    {
        return (is_letter(ch) || is_number(ch) || is_special(ch));
    }
    
    /**
     * @brief Provides a more complete user input: pressing "escape" cancels the
     * input, and support for other features can be easily added.  Provides
     * the same functionality as getline.
     * @param inp the string to store input in.
     * @return A bool: true if the user didn't cancel.
     */
    bool get_user_str(std::string& inp)
    {
        using std::cout;
        using ansi::display_ansi;
        using ansi::cursor::move;
        using ansi::display::clear_line;
        using ansi::cursor::save_pos;
        using ansi::cursor::restore_pos;
        using keyboard::key_code_data;
        
        unsigned int cursor_pos(0); //position of cursor in "inp"
        bool finished(false), canceled(false);
        key_code_data ch;
        
        inp.erase();
        display_ansi(save_pos());
        user_input::cl();
        do
        {
            //update display and cursor:
            display_ansi(restore_pos());
            display_ansi(clear_line(0));
            cout<< inp;
            cout.flush();
            display_ansi(restore_pos());
            if(cursor_pos > 0)
            {
                display_ansi(move(cursor_pos, ansi::cursor::direction::right));
            }
            
            //get user key:
            ch = user_input::getch_funct();
            if(keyboard::is_listed_control(ch))
            {
                using namespace keyboard::code;
                using keyboard::keys;
                
                if(ch == keys[backspace::value])
                {
                    if(cursor_pos > 0)
                    {
                        inp.erase((inp.begin() + (--cursor_pos)));
                    }
                }
                else if(ch == keys[del::value])
                {
                    if(cursor_pos < inp.size())
                    {
                        inp.erase(inp.begin() + cursor_pos);
                    }
                }
                else if(ch == keys[left::value])
                {
                    if(cursor_pos > 0) --cursor_pos;
                }
                else if(ch == keys[right::value])
                {
                    if(cursor_pos < inp.size()) ++cursor_pos;
                }
                else if(ch == keys[end::value])
                {
                    cursor_pos = inp.size();
                }
                else if(ch == keys[home::value])
                {
                    cursor_pos = 0;
                }
                else if(ch == keys[escape::value])
                {
                    inp.erase();
                    canceled = true;
                    finished = true;
                }
            }
            else if(!ch.control_d.empty())
            {
                if(common::is_char(ch.control_d[0]))
                {
                    inp.insert((inp.begin() + cursor_pos), ch.control_d[0]);
                    if(cursor_pos < inp.size()) ++cursor_pos;
                }
                else
                {
                    if(ch.control_d[0] == '\n')
                    {
                        display_ansi(restore_pos());
                        display_ansi(clear_line(0));
                        finished = true;
                    }
                }
            }
        }while(!finished);
        return !canceled;
    }
    
    /**
     * @brief Wait for the user to press any key.
     */
    void wait()
    {
        for(unsigned int x(0); x < 2; ++x) std::cout<< std::endl;
        std::cout<< "Press anything to continue...";
        std::cout.flush();
        user_input::gkey();
    }
    
    /**
     * @brief Displays a string in the center of the screen.
     */
    void center(const std::string& s)
    {
        using std::cout;
        using std::endl;
        
        unsigned int center(((70 / 2) - (s.size() / 2)));
        
        if((s.size() / 2) >= (70 / 2))
        {
            center = 0;
        }
        if(center > 0) cout<< std::string(center, ' ');
        cout<< s<< endl;
    }
    
    void cls()
    {
        ansi::display_ansi(ansi::display::clear_screen(2) + ansi::cursor::jump(1, 1));
    }
    
    /**
     * @brief Propmpts the user with a question that should have a yes/no answer.
     * @param message The message to display.  Defaults to "Are you sure?".
     * @return True if the user picked "yes"
     */
    bool prompt_user(const std::string& message)
    {
        using std::cout;
        using std::endl;
        
        char ch;
        bool finished(false), result(false);
        
        do
        {
            cls();
            cout<< endl;
            center((message.empty() ? "Are you sure?" : message));
            cout<< endl<< endl;
            center("Yes or no?");
            cout.flush();
            
            ch = user_input::gkey();
            ch = std::tolower(ch);
            if(ch == 'y')
            {
                result = true;
                finished = true;
            }
            else if(ch == 'n')
            {
                finished = true;
            }
        }while(!finished);
        return result;
    }
    
    /**
     * @brief distributes an integer equally throughout a list of elements
     * given the element type can be accessed.  Distribution is done by
     * settings each element to (val / v.size()).  The remainder is then added,
     * if there is any.
     * @param val value to distribute.
     * @param v list of elements.
     * @param access The function used to access the number stored at
     * element.  Provided for use with structs, etc...
     */
    template<typename type1, typename type2>
    void distribute_equally(const type1& val, std::vector<type2>& v,
            void (*access)(type2&, type1*&))
    {
        type1 *value(nullptr);
        for(typename std::vector<type2>::iterator it(v.begin()); it != v.end(); ++it)
        {
            access(*it, value);
            (*value) = ((type1)val / (type1)v.size());
        }
        for(unsigned int x = 0; x < ((type1)val % (type1)v.size()); ++x)
        {
            access(v[x], value);
            ++(*value);
        }
    }
    
    /**
     * @brief Returns the number of bytes the current position is from
     * the end of the stream.
     */
    std::istream::pos_type inavail(std::istream& in)
    {
        std::istream::pos_type pos, count(0);
        
        if(in.good())
        {
            pos = in.tellg();
            count = (in.seekg(0, std::ios_base::end).tellg() - pos);
            in.seekg(pos);
        }
        return count;
    }
    
    std::string time_disp(const tdata::time_class& t)
    {
        return (std::to_string(t.hour()) + ":" + std::to_string(t.minute()) + " " + 
                std::string((t.am() ? "am" : "pm")));
    }
    
    std::string date_disp(const tdata::time_class& t)
    {
        return (t.month_name() + " " + std::to_string(t.mday()) + ", " + 
                std::to_string(t.gyear()));
    }
    
    /**
     * @brief Limits a string's maximum size to 'size'.  This is mainly 
     * intended for menu displays.
     * @return a string that has a size <= 'size'.  Appends "..."
     * if the string was resized for context.
     */
    std::string fit_str(const std::string& s, const unsigned int& size)
    {
        std::string temps{s};
        if(temps.size() > size)
        {
            temps.resize(temps.size() - 3);
            temps += "...";
        }
        return temps;
    }
    
    /**
     * @brief Returns true if 's' represents any type of number (float, int, etc...).
     * This function does allow for all types (signed and floating point).
     * @param s the string to test
     * @return true if 's' is a number. 
     */
    bool str_is_num(const std::string& s)
    {
        if(s.empty()) return false;
        if(!common::is_number(s.back())) return false;
        if(s.find('.') != std::string::npos)
        {
            //if there is more than one decimal:
            if(s.find('.') != s.rfind('.')) return false;
        }
        if(s.find('-') != std::string::npos)
        {
            //if there is more than one negative sign:
            if(s.find('-') != s.rfind('-')) return false;
        }
        
        for(std::string::const_iterator iter{s.begin()}; iter != s.end(); ++iter)
        {
            if(((*iter) == '.') || ((*iter) == '-')) continue;
            if(!common::is_number(*iter)) return false;
        }
        return true;
    }
    
    /**
     * @brief Allows the user to modify a date.
     * @param t Date to modify
     * @return True if the user didn't cancel.  If the user cancels, the date
     * remains unchanged.
     */
    bool user_choose_date(tdata::time_class& t) //test
    {
        using keyboard::key_code_data;
        using std::cout;
        using std::endl;
        
        key_code_data key;
        bool finished{false}, cancel{false};
        tdata::time_class temp_time{t};
        unsigned short selected{1};
        
        user_input::cl();
        do
        {
            cls();
            for(unsigned int x{0}; x < v_center::value; ++x) cout<< endl;
            common::center(date_selection_display(temp_time, selected, std::pair<char, char>{'>', '<'}));
            cout<< endl;
            for(unsigned int x{0}; x < 5; ++x) cout<< endl;
            cout<< "\'ESC\' -  Cancel"<< endl;
            cout<< "\'\\\' -  Set to today";
            cout.flush();
            
            key = std::move(user_input::getch_funct());
            
            if(keyboard::is_listed_control(key))
            {
                using namespace keyboard::code;
                using keyboard::keys;
                
                if(key == keys[up::value]) date_selection_add(temp_time, selected, 1);
                else if(key == keys[down::value]) date_selection_add(temp_time, selected, -1);
                else if((key == keys[left::value]) && (selected > 0)) selected = ((selected + 2) % 3);
                else if((key == keys[right::value]) && (selected < 2)) selected = ((selected + 1) % 3);
                else if(key == keys[escape::value])
                {
                    cancel = true;
                    finished = true;
                }
                //todo add page-scrolling (+/- 10 units)
            }
            else if(!key.control_d.empty())
            {
                switch(std::tolower((char)key.control_d[0]))
                {
                    case '\n':
                    {
                        finished = true;
                    }
                    break;
                    
                    case '\\':
                    {
                        temp_time = tdata::current_time();
                    }
                    break;
                    
                    default:
                    {
                    }
                    break;
                }
            }
        }
        while(!finished);
        if(!cancel) t = std::move(temp_time);
        return !cancel;
    }
    
    
}