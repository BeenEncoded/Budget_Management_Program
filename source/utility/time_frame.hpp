#ifndef TIME_FRAME_HPP_INCLUDED
#define TIME_FRAME_HPP_INCLUDED
#include <utility>
#include <iostream>
#include <string>

#include "utility/time_class.hpp"

namespace tdata
{
    typedef class timeframe_class timeframe_class;
    typedef class time_interval_type time_interval_type;
    
    
    std::istream& operator>>(std::istream&, time_interval_type&);
    std::ostream& operator<<(std::ostream&, const time_interval_type&);
    
    std::istream& operator>>(std::istream&, timeframe_class&);
    std::ostream& operator<<(std::ostream&, const timeframe_class&);
    
    
    /**
     * @class time_interval_type
     * @author  Jonathan Whitlock
     * @date 06/25/2015
     * @file time_frame.hpp
     * @brief A time interval type used to store and display time intervals.
     * It also supports operations on a time_class so as to calculate the
     * end of this interval when it is added to a time_class.
     */
    typedef class time_interval_type
    {
    public:
        static constexpr unsigned int max_unit_t{4};
        
        enum unit_t
        {
            day = 0,
            week,
            month,
            year
        };
    
        explicit time_interval_type();
        explicit time_interval_type(const unsigned int&, const unit_t&);
        time_interval_type(const time_interval_type&);
        time_interval_type(time_interval_type&&) noexcept;
        ~time_interval_type();
        
        time_interval_type& operator=(const time_interval_type&);
        time_interval_type& operator=(time_interval_type&&) noexcept;
        
        bool operator==(const time_interval_type&) const;
        bool operator!=(const time_interval_type&) const;
        
        tdata::time_class add_to(const tdata::time_class&) const;
        std::string unit_name() const;
        
        unit_t u;
        unsigned int count;
        
    private:
        //a centralized location for the mathematical algorithms this object uses:
        void add_month(tdata::time_class&) const;
        void sub_month(tdata::time_class&) const;
        void add_year(tdata::time_class&) const;
        void sub_year(tdata::time_class&) const;
        
        tdata::time_class op_on(const tdata::time_class&, const bool&) const;
    } time_interval_type;
    
    /**
     * @class timeframe_class
     * @author Jonathan Whitlock
     * @date 06/25/2015
     * @file time_frame.hpp
     * @brief Used to store, manipulate, and compare a frame of time.  Uses 
     * time_class to calculate and store the beginning and end times.
     */
    typedef class timeframe_class
    {
    public:
        explicit timeframe_class();
        explicit timeframe_class(const unsigned int&, const time_interval_type::unit_t&);
        explicit timeframe_class(const time_interval_type&);
        explicit timeframe_class(const time_class&, const time_interval_type&);
        timeframe_class(const timeframe_class&);
        timeframe_class(timeframe_class&&) noexcept;
        ~timeframe_class();
        
        timeframe_class& operator=(const timeframe_class&);
        timeframe_class& operator=(timeframe_class&&) noexcept;
        
        bool operator==(const timeframe_class&) const;
        bool operator!=(const timeframe_class&) const;
        
        const time_interval_type& cinterval() const noexcept;
        bool overlaps(const timeframe_class&) const;
        
        friend std::istream& operator>>(std::istream&, timeframe_class&);
        friend std::ostream& operator<<(std::ostream&, const timeframe_class&);
        
        time_class beg;
    private:
        time_interval_type interval;
        
        bool is_within_frame(const time_class&) const;
        
    } timeframe_class;
}

#endif