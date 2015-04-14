#ifndef SCROLL_DISPLAY_HPP_INCLUDED
#define SCROLL_DISPLAY_HPP_INCLUDED
#include <vector>
#include <string>
#include <assert.h>

#include "global_defines.hpp"
#include "program_settings.hpp"

namespace scrollDisplay
{
    typedef class scroll_display_class scroll_display_class;
    typedef struct window_data window_data;
    typedef struct position_data position_data;
    
    
    typedef struct window_data
    {
        void operator=(const window_data& wd)
        {
            if(this != & wd)
            {
                this->beg = wd.beg;
                this->size = wd.size;
            }
        }
        
        signed long size = 15, beg = 0;
    } window_data;
    
    typedef struct position_data
    {
        void operator=(const position_data& pd)
        {
            if(this != &pd)
            {
                this->part = pd.part;
                this->whole = pd.whole;
            }
        }
        
        short part = 0;
        unsigned int whole = 0;
    } position_data;
    
    /* scroll_display_class: a class designed to manage a window and make 
     * it easy to create a scroll display.*/
    typedef class scroll_display_class
    {
    public:
        explicit scroll_display_class() : display(NULL), 
                wind(), 
                pos() {}
        
        explicit scroll_display_class(std::vector<std::string>& d) : display(&d),
                    wind(),
                    pos()
        {
        }
        
        ~scroll_display_class(){}
        
        void operator=(const scroll_display_class& sdc)
        {
            if(this != &sdc)
            {
                assert(sdc.display != NULL);
                this->display = sdc.display;
                this->pos = sdc.pos;
                this->wind = sdc.wind;
            }
        }
        
        bool mv_up();
        bool mv_down();
        bool pg_up();
        bool pg_down();
        
        /* Returns the "window" as a list of strings.  This can
         be displayed. */
        std::vector<std::string> window();
        
        /* Returns the current position in the window.  part
         represents the position relative to the window, and
         whole represents the position relative to the entire
         display.*/
        const position_data& gpos();
        
        /* Returns the size of the scroll display.  can be 
         modified directly. */
        long &window_size()
        {
            return this->wind.size;
        }
        
        /* Returns the first position of the window, relative to the 
         entire display. */
        const signed long& window_beg()
        {
            assert(this->display != NULL);
            this->sync();
            return this->wind.beg;
        }
        
    private:
        std::vector<std::string> *display;
        window_data wind;
        position_data pos;
        
        void sync();
        bool scroll_up();
        bool scroll_down();
        bool scroll_pg_up();
        bool scroll_pg_down();
        
        /* Calculates the last possible index that can
         be safely referenced from the pointed vector, based on where the window
         is positioned.*/
        signed long end_pos() const
        {
            assert(this->display != NULL);
            signed long temp(this->wind.beg + (this->wind.size - 1));
            if(this->display->size() > 0)
            {
                if(temp > 0)
                {
                    if(unsigned(temp) >= this->display->size()) temp = (this->display->size() - 1);
                }
                if(temp < this->wind.beg)
                {
                    throw "Error:  signed long end_pos() const (end_pos < wind.begin)!!!";
                }
            }
            else
            {
                temp = (-1);
            }
            return temp;
        }
        
        /* Returns the size of the window which has been "resized"
         to fit within the pointed vector.  Does not reflect
         the value of wind.size.  (this should be <= wind.size)*/
        signed long current_wsize() const
        {
            return ((this->end_pos() - this->wind.beg) + 1);
        }
        
        
    } scroll_display_class;
    
    /** An abstraction of the scroll display class meant to
     * associate a list of arbitray elements with the window. */
    template<class type>
    class window_data_class
    {
    public:
        explicit window_data_class();
        explicit window_data_class(std::vector<type>&, 
                void (*)(const std::vector<type>&, std::vector<std::string>&));
        ~window_data_class();
        
        window_data_class<type>& operator=(const window_data_class<type>&);
        bool operator==(const window_data_class<type>& w) const noexcept;
        bool operator!=(const window_data_class<type>& w) const noexcept;
        
        void remove_selected();
        type& selected();
        scroll_display_class& win();
        
    private:
        void update();
    
        std::vector<type> *data;
        void (*update_display)(const std::vector<type>&, std::vector<std::string>&);
        std::vector<std::string> display;
        scroll_display_class window;
        
    };
    
}


#endif