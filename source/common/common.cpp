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

namespace
{
    std::string parent_path(const std::string&);
    
    
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
     * input, and support for other features can be easily added.
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
                    if(ch.control_d[0] == '\n') finished = true;
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
     * @brief Safely retrieves a line from a file given a specified delimiter.  
     * In case of failure, it will leave the stream in the state it was in
     * before the function call.  Returns true/false based on success.
     * @param in std::istream to read from
     * @param t object to read into
     * @param delimiter delimiter to read to
     * @return bool true if the read was a success, false otherwise.
     */
    template<class type>
    bool safe_getline(std::istream& in, type& t, const char& delimiter)
    {
        std::string temps;
        bool success(false);
        std::istream::pos_type previous_position(in.tellg());
        
        t = type();
        
        if(in.good())
        {
            std::getline(in, temps, delimiter);
            if(in.good())
            {
                std::stringstream ss;
                ss<< temps;
                ss>> t;
                success = true;
            }
            else if(!in.eof() && in.fail())
            {
                /* So, here we are: we didn't reach the end of the file, 
                 but somwhere there was a big mistake... the program will
                 now attempt to salvage the situation: */
                in.seekg(previous_position);
                success = false;
            }
        }
        return success;
    }
    
    /**
     * @brief A specialization of safe_getline for strings.
     * @param in std::istream to read from
     * @param s the string to read into
     * @param delimiter the delimiter to read up to.
     * @return a bool: true if the read was a success, false otherwise.
     */
    template<>
    bool safe_getline<std::string>(std::istream& in, std::string& s, const char& delimiter)
    {
        bool success(false);
        std::istream::pos_type previous_position(in.tellg());
        
        s.erase();
        
        if(in.good())
        {
            std::getline(in, s, delimiter);
            if(in.good()) success = true;
            else if(!in.eof() && in.fail())
            {
                /* So, here we are: we didn't reach the end of the file, 
                 but somwhere there was a big mistake... the program will
                 now attempt to salvage the situation: */
                in.seekg(0, std::ios::beg);
                in.seekg(previous_position);
                success = false;
            }
        }
        return success;
    }
    
    template<typename type>
    std::istream& in_mem(std::istream& in, type& t)
    {
        t = type();
        char *mem(new char[sizeof(type)]);
        
        in.peek();
        if(in.good())
        {
            unsigned int x(0);
            for(; ((x < sizeof(type)) && in.good()); ++x)
            {
                mem[x] = in.get();
            }
            if(((x + 1) < sizeof(type)) && !in.fail()) in.setstate(std::ios_base::failbit);
            memcpy(&t, mem, sizeof(type));
        }
        delete[] mem;
        return in;
    }
    
    template std::istream& in_mem(std::istream&, unsigned long long&);
    template std::istream& in_mem(std::istream&, long long&);
    template std::istream& in_mem(std::istream&, std::size_t&);
    
    template<typename type>
    std::ostream& out_mem(std::ostream& out, const type& t)
    {
        char *mem(new char[sizeof(type)]);
        
        memcpy(mem, &t, sizeof(type));
        if(out.good())
        {
            unsigned int x(0);
            for(; ((x < sizeof(type)) && out.good()); ++x)
            {
                out<< mem[x];
            }
            if(((x + 1) < sizeof(type)) && !out.fail()) out.setstate(std::ios_base::failbit);
        }
        delete[] mem;
        return out;
    }
    
    template std::ostream& out_mem(std::ostream&, const unsigned long long&);
    template std::ostream& out_mem(std::ostream&, const long long&);
    template std::ostream& out_mem(std::ostream&, const std::size_t&);
    
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
     * @brief Loads an object from a file.  Type requirements include an overloaded 
     * operator>>
     * @param path The full path to the file.
     * @param t The object to read the data into.
     * @return result_data<>.
     */
    template<typename type>
    result_data<> load_from_file(const std::string& path, type& t)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        
        t = type();
        
        result_data<> result;
        result.message = "ERROR IN template<typename type> bool load_from_file(\
const std::string&, type&):  ";
        
        if(is_symlink(path).value || !is_file(path).value)
        {
            result.message += ("\"" + path + "\" is a symlink or doesn't exist!  \
Could not load it.");
            return result;
        }
        
        std::ifstream in(path, std::ios::in);
        
        in.peek();
        if(in.good())
        {
            in>> t;
            if(in.fail())
            {
                result.message += "Stream operation failed!";
                return result;
            }
        }
        else
        {
            result.message += "Stream could not be opened!";
            return result;
        }
        in.close();
        result.success = true;
        result.message.erase();
        return result;
    }
    
    template result_data<> load_from_file<data::budget_data>(const std::string&, data::budget_data&);
    
    /**
     * @brief Saves an object to a file.  Type requirements include an overloaded
     * operator<<
     * @param path The full path to the file.  WARNING:  If the parent folders
     * don't already exist, then they will be created.
     * @param t The object to write.
     * @return result_data<>.
     */
    template<typename type>
    result_data<> save_to_file(const std::string& path, const type& t)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::create_folder;
        
        result_data<> result;
        
        result.message = "ERROR IN: template<typename type> \
bool save_to_file(const std::string&, const type&):  ";
        {
            std::string f(parent_path(path));
            if(!create_folder(f).value)
            {
                if(!is_folder(f).value)
                {
                    result.message += ("\"" + f + "\" Could not be created, and \
doesn't exist!");
                    return result;
                }
            }
        }
        
        std::ofstream out(path, std::ios::out);
        if(out.good()) out<< t;
        if(out.fail())
        {
            if(out.is_open()) out.close();
            result.message += "Stream operation failed!";
            return result;
        }
        result.success = true;
        result.message.erase();
        return result;
    }
    
    template result_data<> save_to_file<data::budget_data>(const std::string&, const data::budget_data&);
    
    /**
     * @brief Writes a vector of type 'type' to a stream.  Garunteed not to
     * cause delimiter collisions.  Intended to use in operators.
     * @param out Stream to write to.
     * @param v The vector to write.
     * @return The stream.
     */
    template<typename type>
    std::ostream& write_vector(std::ostream& out, const std::vector<type>& v)
    {
        if(out.good())
        {
            out_mem<std::size_t>(out, v.size());
            for(unsigned int x(0); ((x < v.size()) && out.good()); ++x)
            {
                out<< v[x];
            }
        }
        return out;
    }
    
    template std::ostream& write_vector<data::budget_data>(std::ostream&, const std::vector<data::budget_data>&);
    template std::ostream& write_vector<data::money_alloc_data>(std::ostream&, const std::vector<data::money_alloc_data>&);
    
    /**
     * @brief Reads a vector of type 'type' from a stream.  Garunteed not to
     * cause delimiter collisions. Intended to use in operators.
     * @param in The stream to read from.
     * @param v The vector to store the data in.
     * @return The stream.
     */
    template<typename type>
    std::istream& read_vector(std::istream& in, std::vector<type>& v)
    {
        v.erase(v.begin(), v.end());
        in.peek();
        if(in.good())
        {
            std::size_t size(0);
            
            in_mem<std::size_t>(in, size);
            for(std::size_t x(0); ((x < size) && in.good() && (in.peek() != EOF)); ++x)
            {
                v.push_back(type());
                in>> v.back();
                if(in.fail() && !in.eof()) v.pop_back();
                in.peek();
            }
            in.peek();
        }
        return in;
    }
    
    template std::istream& read_vector<data::budget_data>(std::istream&, std::vector<data::budget_data>&);
    template std::istream& read_vector<data::money_alloc_data>(std::istream&, std::vector<data::money_alloc_data>&);
    
    /**
     * @brief Peeks 'count' characters.  Can be used to check multi-char delimiters.
     * The stream is garunteed to be left in the state it was found.
     * @param in stream to peek
     * @param count number of characters to peek.
     * @return A string of the next 'count' characters.  Not garunteed to
     * be of size 'count'.  If the end of the stream is reached before 'count'
     * characters can be read, the operation is aborted, and the string is returned.
     */
    std::string peek_string(std::istream& in, const unsigned int& count)
    {
        std::string temps;
        
        in.peek();
        if(in.good())
        {
            std::istream::pos_type pos(in.tellg());
            for(unsigned int x(0); ((x < count) && (in.peek() != EOF) && in.good()); ++x) temps += in.get();
            if(!in.good()) in.clear();
            in.seekg(pos);
        }
        return temps;
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
    
    
}