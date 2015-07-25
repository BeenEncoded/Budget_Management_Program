#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <assert.h>
#include <stdexcept>
#include <exception>
#include <utility>

#include "scroll_display.hpp"
#include "utility/time_frame.hpp"
#include "data/budget_data.hpp"

#ifndef ethrow
#define ethrow(MSG) throw std::runtime_error(std::string(std::string(__FILE__) + \
"    Exception thrown at line " + std::to_string(__LINE__) + \
": " + std::string(MSG)))
#endif

namespace
{
    template<class type1, class type2>
    inline type2 conv(const type1& t1)
    {
        std::stringstream ss;
        type2 t2;
        ss<< t1;
        ss>> t2;
        return t2;
    }
    
    
}

//scroll_display_class member functions:
namespace scrollDisplay
{
    scroll_display_class::scroll_display_class() : 
            display(NULL),
            wind(), 
            pos()
    {
    }
    
    scroll_display_class::scroll_display_class(std::vector<std::string>& d) : 
            display(&d),
            wind(),
            pos()
    {
    }
    
    scroll_display_class::scroll_display_class(const scroll_display_class& s) : 
            display(s.display),
            wind(s.wind),
            pos(s.pos)
    {
    }
    
    scroll_display_class::scroll_display_class(scroll_display_class&& s) noexcept : 
            display(std::move(s.display)),
            wind(std::move(s.wind)),
            pos(std::move(s.pos))
    {
    }
    
    scroll_display_class::~scroll_display_class()
    {
    }
    
    scroll_display_class& scroll_display_class::operator=(const scroll_display_class& s)
    {
        if(this != &s)
        {
            this->display = s.display;
            this->wind = s.wind;
            this->pos = s.pos;
        }
        return *this;
    }
    
    scroll_display_class& scroll_display_class::operator=(scroll_display_class&& s) noexcept
    {
        this->display = std::move(s.display);
        this->wind = std::move(s.wind);
        this->pos = std::move(s.pos);
        return *this;
    }
    
    /** @brief True/false based on whether it moved the window or not. */
    bool scroll_display_class::scroll_up()
    {
        this->sync();
        bool success(false);
        if(this->wind.size > 0)
        {
            if(this->wind.beg > 0)
            {
                success = true;
                this->wind.beg--;
            }
        }
        return success;
    }
    
    /** @brief True/false based on whether it moved the window or not. */
    bool scroll_display_class::scroll_down()
    {
        this->sync();
        bool success(false);
        if(this->wind.size > 0)
        {
            if(this->end_pos() > 0)
            {
                if((unsigned(this->end_pos()) + 1) < this->display->size())
                {
                    this->wind.beg++;
                    success = true;
                }
            }
        }
        return success;
    }
    
    /** @brief True/false based on whether it moved the window or not. */
    bool scroll_display_class::scroll_pg_down()
    {
        this->sync();
        bool success(false);
        if(this->wind.size > 0)
        {
            if(this->end_pos() > 0)
            {
                if(unsigned((this->end_pos() + (this->wind.size - 1))) < this->display->size())
                {
                    success = true;
                    this->wind.beg += (this->wind.size - 1);
                }
            }
        }
        return success;
    }
    
    /** @brief True/false based on whether it moved the window or not. */
    bool scroll_display_class::scroll_pg_up()
    {
        this->sync();
        bool success(false);
        if(this->wind.beg >= (this->wind.size - 1))
        {
            success = true;
            this->wind.beg -= (this->wind.size - 1);
        }
        else
        {
            if(this->wind.beg != 0)
            {
                this->wind.beg = 0;
                success = true;
            }
        }
        return success;
    }
    
    /** @brief Returns the "window" as a list of strings.  This can
     * be displayed. */
    std::vector<std::string> scroll_display_class::window()
    {
        this->sync();
        using namespace std;
        vector<string> tempv;
        if(this->wind.size > 0)
        {
            for(unsigned int x = this->wind.beg; x <= this->end_pos(); x++)
            {
                tempv.push_back(this->display->at(x));
            }
        }
        return tempv;
    }
    
    /** @brief Keeps the window within the bounds of the display, the
     * display the proper size, and the positions within bounds.  The window
     * positions relative to the entire vector are derived from pos.whole, so
     * only pos.whole is needed to move the position. */
    void scroll_display_class::sync()
    {
        if(this->wind.size < 0) this->wind.size *= (-1);
        assert(this->display != NULL);
        
        if(this->display->size() == 0)
        {
            this->wind.beg = 0;
            this->pos.whole = 0;
            this->pos.part = 0;
        }
        
        //make sure the beg is within defined bounds
        if((this->wind.size > 0) && (this->display->size() > 0))
        {
            //make sure the current position is within the vector:
            if(this->pos.whole >= this->display->size()) this->pos.whole = (this->display->size() - 1);
            
            //make sure the current position is in the field of view:
            if(this->pos.whole < this->wind.beg) this->wind.beg = this->pos.whole;
            if((this->pos.whole > this->end_pos()) && (this->end_pos() > 0))
            {
                this->wind.beg = (this->pos.whole - (this->wind.size - 1));
            }
            
            //reset the part position:
            this->pos.part = (this->pos.whole - this->wind.beg);
            
            //make sure that we can always have a full window, within the vector's bounds:
            if((unsigned(this->end_pos()) == (this->display->size() - 1)) && 
                    (this->current_wsize() < this->wind.size) && 
                    ((unsigned)this->display->size() >= this->wind.size))
            {
                this->wind.beg = (this->display->size() - this->wind.size);
            }
            
            //correct if beg is < 0
            if(this->wind.beg < 0) this->wind.beg = 0;
        }
    }
    
    bool scroll_display_class::mv_up()
    {
        this->sync();
        bool success(false);
        if(this->display->size() > 0)
        {
            if(this->pos.whole > 0)
            {
                this->pos.whole--;
                this->sync();
                success = true;
            }
        }
        return success;
    }
    
    bool scroll_display_class::mv_down()
    {
        this->sync();
        bool success(false);
        if(this->display->size() > 0)
        {
            if((this->pos.whole + 1) < this->display->size())
            {
                this->pos.whole++;
                this->sync();
                success = true;
            }
        }
        return success;
    }
    
    bool scroll_display_class::pg_up()
    {
        this->sync();
        bool success(false);
        if(this->display->size() > 0)
        {
            
            /* So, first we need to check if the "previous window" is within
             the vector's bounds.  If it is, then we simply pg-up, otherwise
             we just jump to the first element.*/
            if(this->pos.whole >= (this->wind.size - 1))
            {
                this->pos.whole -= (this->wind.size - 1);
                this->sync();
                success = true;
            }
            else
            {
                if(this->pos.whole != 0)
                {
                    this->pos.whole = 0;
                    this->sync();
                    success = true;
                }
            }
        }
        return success;
    }
    
    bool scroll_display_class::pg_down()
    {
        this->sync();
        bool success(false);
        if(this->display->size() > 0)
        {
            
            /* So, basically, we need to check if the "next window" is within 
             the display vector's bounds.  If it is, then we simply page-down,
             otherwise, we can just jump to the end. */
            if(unsigned(((this->display->size() - 1) - this->pos.whole)) >= (this->wind.size - 1))
            {
                this->pos.whole += (this->wind.size - 1);
                success = true;
                this->sync();
            }
            else
            {
                if(this->pos.whole < (this->display->size() - 1))
                {
                    this->pos.whole = (this->display->size() - 1);
                    this->sync();
                    success = true;
                }
            }
        }
        return success;
    }
    
    /**
     * @brief Jumps to position "pos".
     * @return true if the display was modified.
     */
    bool scroll_display_class::jmp(const unsigned int& pos)
    {
        this->sync();
        if(this->display->empty()) return false;
        if(pos >= this->display->size()) return false;
        
        this->pos.whole = pos;
        this->sync();
        return true;
    }
    
    /** @brief Returns the current position in the window.  part
     * represents the position relative to the window, and
     * whole represents the position relative to the entire
     * display.*/
    const position_data& scroll_display_class::gpos()
    {
        this->sync();
        return this->pos;
    }
    
    /** @brief Returns the size of the scroll display.  can be 
     * modified directly. */
    long& scroll_display_class::window_size()
    {
        return this->wind.size;
    }
    
    /** @brief Returns the first position of the window, relative to the 
     * entire display. */
    const signed long& scroll_display_class::window_beg()
    {
        this->sync();
        return this->wind.beg;
    }
    
    /** @brief Calculates the last possible index that can
     * be safely referenced from the pointed vector, based on where the window
     * is positioned.*/
    signed long scroll_display_class::end_pos() const
    {
        assert(this->display != NULL);
        signed long temp(this->wind.beg + (this->wind.size - 1));
        if(!this->display->empty())
        {
            if(temp > 0)
            {
                if(unsigned(temp) >= this->display->size()) temp = (this->display->size() - 1);
            }
            else if(temp < this->wind.beg)
            {
                ethrow("Error:  signed long end_pos() const (end_pos < wind.begin)!!!");
            }
        }
        else
        {
            temp = (-1);
        }
        return temp;
    }
    
    /** @brief Returns the (real) size of the window which has been "resized"
     * to fit within the pointed vector.  Does not reflect
     * the value of wind.size.  (this should be <= wind.size)*/
    signed long scroll_display_class::current_wsize() const
    {
        return ((this->end_pos() - this->wind.beg) + 1);
    }
    
    
}

//position_data member functions:
namespace scrollDisplay
{
    position_data::position_data() : 
            part(0),
            whole(0)
    {
    }
    
    position_data::position_data(const position_data& p) : 
            part(p.part),
            whole(p.whole)
    {
    }
    
    position_data::position_data(position_data&& p) noexcept : 
            part(std::move(p.part)),
            whole(std::move(p.whole))
    {
    }
    
    position_data::~position_data()
    {
    }
    
    position_data& position_data::operator=(const position_data& p)
    {
        if(this != &p)
        {
            this->part = p.part;
            this->whole = p.whole;
        }
        return *this;
    }
    
    position_data& position_data::operator=(position_data&& p) noexcept
    {
        this->part = std::move(p.part);
        this->whole = std::move(p.whole);
        return *this;
    }
    
    
}

//window_data member functions:
namespace scrollDisplay
{
    window_data::window_data() : 
            size(15),
            beg(0)
    {
    }
    
    window_data::window_data(const window_data& w) : 
            size(w.size),
            beg(w.beg)
    {
    }
    
    window_data::window_data(window_data&& w) noexcept : 
            size(std::move(w.size)),
            beg(std::move(w.beg))
    {
    }
    
    window_data::~window_data()
    {
    }
    
    window_data& window_data::operator=(const window_data& w)
    {
        if(this != &w)
        {
            this->size = w.size;
            this->beg = w.beg;
        }
        return *this;
    }
    
    window_data& window_data::operator=(window_data&& w) noexcept
    {
        this->size = std::move(w.size);
        this->beg = std::move(w.beg);
        return *this;
    }
    
    
}

//window_data_class
namespace scrollDisplay
{
    template<class type>
    window_data_class<type>::window_data_class() : data(NULL), 
            update_display(NULL),
            display(),
            window(display)
    {
    }
    
    template<class type>
    window_data_class<type>::window_data_class(std::vector<type>& d, 
            void (*conv)(const std::vector<type>&, std::vector<std::string>&)) : 
            data(&d),
            update_display(conv),
            display(),
            window(display)
    {
        if(data == nullptr) ethrow("You can not initialize window_data_class with null data!");
        this->update();
    }
    
    template<class type>
    window_data_class<type>::window_data_class(const window_data_class<type>& w) : 
            data(w.data),
            update_display(w.update_display),
            display(w.display),
            window(w.window)
    {
    }
    
    template<class type>
    window_data_class<type>::window_data_class(window_data_class<type>&& w) noexcept : 
            data(std::move(w.data)),
            update_display(std::move(w.update_display)),
            display(std::move(w.display)),
            window(std::move(w.window))
    {
    }
    
    template<class type>
    window_data_class<type>::~window_data_class()
    {
    }
    
    template<class type>
    window_data_class<type>& window_data_class<type>::operator=(const window_data_class<type>& w)
    {
        if(this != &w)
        {
            this->display.clear();
            this->data = w.data;
            this->update_display = w.update_display;
            this->window = w.window;
            this->display = w.display;
        }
        return *this;
    }
    
    template<class type>
    window_data_class<type>& window_data_class<type>::operator=(window_data_class<type>&& w) noexcept
    {
        this->data = std::move(w.data);
        this->update_display = std::move(w.update_display);
        this->display = std::move(w.display);
        this->window = std::move(w.window);
        return *this;
    }
    
    template<class type>
    bool window_data_class<type>::operator==(const window_data_class<type>& w) const noexcept
    {
        return ((this->data == w.data) && 
                (this->display == w.display) && 
                (this->update_display == w.update_display));
    }
    
    template<class type>
    bool window_data_class<type>::operator!=(const window_data_class<type>& w) const noexcept
    {
        return !(this->operator==(w));
    }
    
    template<class type>
    void window_data_class<type>::remove_selected()
    {
        if(!this->data->empty())
        {
            this->data->erase(this->data->begin() + this->window.gpos().whole);
            this->update();
        }
    }
    
    template<class type>
    type& window_data_class<type>::selected()
    {
        if(this->data == nullptr) ethrow("Invalid reference to null pointer.");
        this->update();
        return (*(this->data))[this->window.gpos().whole];
    }
    
    /**
     * @brief Used when you can't reference the selected object directly.  Such
     * a case would be when you plan on modifying the vector that this class
     * holds a pointer to.
     */
    template<class type>
    type window_data_class<type>::cselected()
    {
        type t{this->selected()};
        return t;
    }
    
    template<class type>
    scroll_display_class& window_data_class<type>::win()
    {
        this->update();
        return this->window;
    }
    
    template<class type>
    void window_data_class<type>::update()
    {
        if(this->data == nullptr)
        {
            ethrow("Can not update window_data_class with null data!");
        }
        this->update_display(*(this->data), this->display);
    }
    
    template<class type>
    const std::vector<type>* const window_data_class<type>::gdata() const
    {
        return this->data;
    }
    
    
    template class window_data_class<tdata::time_interval_type::unit_t>;
    template class window_data_class<std::string>;
    template class window_data_class<data::money_alloc_data>;
}

namespace
{
    std::string limit_length(const std::string&, const unsigned int&);
    
    
    
    inline std::string limit_length(const std::string& s, const unsigned int& len)
    {
        if(s.empty()) return s;
        if(s.size() < len) return s;
        
        std::string temps(s);
        
        if(temps.size() > len)
        {
            temps.resize((len - 3));
            temps += "...";
        }
        return temps;
    }
    
    
}

//misc functions
namespace scrollDisplay
{
    /**
     * @brief Displays a scroll window.  Does not append a new line.
     * @param win The window to show
     * @param bracket_char bracket characters to use for the selected element.
     * @param spec_bracket_char bracket characters to use for multiple selected
     * elements.
     * @param spec_selected selected elements.
     */
    template<typename type>
    void display_window(
            window_data_class<type>& win, 
            const std::pair<char, char>& bracket_char, 
            const std::pair<char, char>& spec_bracket_char, 
            const std::unordered_set<unsigned int>& spec_selected)
    {
        using std::cout;
        using std::endl;
        
        if(!win.gdata()->empty())
        {
            constexpr unsigned int win_width(70);
            
            scroll_display_class *scroll_window(&win.win()); //this way we don't force an update on each access
            unsigned int e_before(scroll_window->window_beg()), 
                    e_after((win.gdata()->size() - (scroll_window->window_beg() + scroll_window->window_size())));
            std::vector<std::string> disp(std::move(scroll_window->window()));
            bool is_selected(false), is_special_selected(false);
            
            if(e_before > 0)
            {
                cout<< std::string(((win_width / 2) - (std::to_string(e_before).size() / 2)), ' ')<< e_before<< endl;
                cout<< std::string(win_width, '^')<< endl;
            }
            else
            {
                cout<< endl;
                cout<< std::string(70, '-')<< endl;
            }
            for(unsigned int x(0); x < disp.size(); ++x)
            {
                is_special_selected = (spec_selected.find((x + scroll_window->window_beg())) != spec_selected.end());
                is_selected = ((signed short)x == scroll_window->gpos().part);
                
                cout<< (is_special_selected ? spec_bracket_char.first : ' ');
                cout<< (is_selected ? bracket_char.first : ' ');
                
                disp[x] = limit_length(disp[x], (win_width - 4));
                if(disp[x].size() < (win_width - 4)) disp[x] += std::string(((win_width - 4) - disp[x].size()), ' ');
                cout<< disp[x];
                
                cout<< (is_selected ? bracket_char.second : ' ');
                if(is_special_selected) cout<< spec_bracket_char.second;
                
                cout<< endl;
            }
            //to create a constant sized boundry, write a number of newlines if the display isn't full:
            for(unsigned int x(0); x < (scroll_window->window_size() - disp.size()); ++x)
            {
                cout<< endl;
            }
            if((e_after > 0) && (win.gdata()->size() > (unsigned)(scroll_window->window_beg() + scroll_window->window_size())))
            {
                cout<< std::string(70, 'V')<< endl;
                cout<< std::string(((70 / 2) - (std::to_string(e_after).size() / 2)), ' ')<< e_after;
            }
            else
            {
                cout<< std::string(70, '-');
                cout<< endl;
            }
            cout.flush();
        }
    }
    
    template void display_window<std::string>(
            window_data_class<std::string>&,
            const std::pair<char, char>&,
            const std::pair<char, char>&,
            const std::unordered_set<unsigned int>&);
            
    template void display_window<data::money_alloc_data>(
            window_data_class<data::money_alloc_data>&,
            const std::pair<char, char>&,
            const std::pair<char, char>&,
            const std::unordered_set<unsigned int>&);
    
    
    /**
     * @brief Displays a scroll window.  Does not append a new line.
     * @param win The window to show
     * @param bracket_char bracket characters to use for the selected element.
     */
    template<typename type>
    void display_window(
            window_data_class<type>& win,
            const std::pair<char, char>& bracks)
    {
        display_window<type>(win, bracks, std::pair<char, char>('>', '<'), std::unordered_set<unsigned int>());
    }
    
    template void display_window(window_data_class<std::string>&,
            const std::pair<char, char>&);
    template void display_window<data::money_alloc_data>(window_data_class<data::money_alloc_data>&, const std::pair<char, char>&);
    template void display_window<tdata::time_interval_type::unit_t>(window_data_class<tdata::time_interval_type::unit_t>&, const std::pair<char, char>&);
    
    /**
     * @brief Displays a scroll window.  Does not append a new line.
     * @param win The window to show
     */
    template<typename type>
    void display_window(window_data_class<type>& w)
    {
        display_window(w, std::pair<char, char>('[', ']'));
    }
    
    template void display_window<std::string>(window_data_class<std::string>&);
    template void display_window<data::money_alloc_data>(window_data_class<data::money_alloc_data>&);
    
    
}

