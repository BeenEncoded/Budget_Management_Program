#include <string>
#include <sstream>
#include <vector>
#include <assert.h>

#include "scroll_display.hpp"
#include "snapshot_class.hpp"
#include "global_defines.hpp"

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

namespace scrollDisplay
{
    /* True/false based on whether it moved the window or not. */
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
    
    /* True/false based on whether it moved the window or not. */
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
    
    /* True/false based on whether it moved the window or not. */
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
    
    /* True/false based on whether it moved the window or not. */
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
    
    /* Keeps the window within the bounds of the display, the
     display the proper size, and the positions within bounds. */
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
            if((unsigned(this->end_pos()) == (this->display->size() - 1)) && (this->current_wsize() < this->wind.size))
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
    
    const position_data& scroll_display_class::gpos()
    {
        this->sync();
        return this->pos;
    }
    
    
}

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
    
    template class window_data_class<snapshot::snapshot_data>;
    template class window_data_class<settings::regex_data>;
    template class window_data_class<std::string>;
}

