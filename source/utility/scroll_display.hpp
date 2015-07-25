#ifndef SCROLL_DISPLAY_HPP_INCLUDED
#define SCROLL_DISPLAY_HPP_INCLUDED
#include <vector>
#include <string>
#include <assert.h>
#include <utility>
#include <unordered_set>

namespace scrollDisplay
{
    typedef class scroll_display_class scroll_display_class;
    typedef struct window_data window_data;
    typedef struct position_data position_data;
    template<class type> class window_data_class;
    
    template<typename type> void display_window(
            window_data_class<type>&, 
            const std::pair<char, char>&, 
            const std::pair<char, char>&, 
            const std::unordered_set<unsigned int>&);
    
    template<typename type> void display_window(window_data_class<type>&, const std::pair<char, char>&);
    template<typename type> void display_window(window_data_class<type>&);
    
    
    typedef struct window_data
    {
        explicit window_data();
        window_data(const window_data&);
        window_data(window_data&&) noexcept;
        ~window_data();
        
        window_data& operator=(const window_data&);
        window_data& operator=(window_data&&) noexcept;
        
        signed long size, beg;
    } window_data;
    
    typedef struct position_data
    {
        explicit position_data();
        position_data(position_data&&) noexcept;
        position_data(const position_data&);
        ~position_data();
        
        position_data& operator=(const position_data&);
        position_data& operator=(position_data&&) noexcept;
        
        short part;
        unsigned int whole;
    } position_data;
    
    
    /* scroll_display_class: a class designed to manage a window and make 
     * it easy to create a scroll display by taking over the management of 
     * window position data. */
    typedef class scroll_display_class
    {
    public:
        explicit scroll_display_class();
        explicit scroll_display_class(std::vector<std::string>&);
        scroll_display_class(const scroll_display_class&);
        scroll_display_class(scroll_display_class&&) noexcept;
        
        ~scroll_display_class();
        
        scroll_display_class& operator=(const scroll_display_class&);
        scroll_display_class& operator=(scroll_display_class&&) noexcept;
        
        bool mv_up();
        bool mv_down();
        bool pg_up();
        bool pg_down();
        bool jmp(const unsigned int&);
        
        std::vector<std::string> window();
        const position_data& gpos();
        long &window_size();
        const signed long& window_beg();
        
    private:
        std::vector<std::string> *display;
        window_data wind;
        position_data pos;
        
        void sync();
        bool scroll_up();
        bool scroll_down();
        bool scroll_pg_up();
        bool scroll_pg_down();
        signed long end_pos() const;
        signed long current_wsize() const;
        
    } scroll_display_class;
    
    /** An abstraction of the scroll display class meant to
     * associate a list of arbitray elements with the window.  Can be
     * used to create an arbitrary window display. */
    template<class type>
    class window_data_class
    {
    public:
        explicit window_data_class();
        explicit window_data_class(std::vector<type>&, 
                void (*)(const std::vector<type>&, std::vector<std::string>&));
        window_data_class(const window_data_class<type>&);
        window_data_class(window_data_class<type>&&) noexcept;
        ~window_data_class();
        
        window_data_class<type>& operator=(const window_data_class<type>&);
        window_data_class<type>& operator=(window_data_class<type>&&) noexcept;
        bool operator==(const window_data_class<type>& w) const noexcept;
        bool operator!=(const window_data_class<type>& w) const noexcept;
        
        void remove_selected();
        type& selected();
        type cselected();
        scroll_display_class& win();
        const std::vector<type>* const gdata() const;
        
    private:
        void update();
    
        std::vector<type> *data;
        void (*update_display)(const std::vector<type>&, std::vector<std::string>&);
        std::vector<std::string> display;
        scroll_display_class window;
        
    };
    
    
}


#endif