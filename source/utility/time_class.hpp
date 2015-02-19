#ifndef TIME_CLASS_HPP_INCLUDED
#define TIME_CLASS_HPP_INCLUDED
#include <ctime>
#include <iostream>
#include <chrono>
#include <unistd.h>

#include "globals.hpp"

namespace tdata
{
    //time constants: 
    namespace t_const
    {
        typedef const_int_type<60>                   minute;
        typedef const_int_type<(minute::value * 60)> hour;
        typedef const_int_type<(hour::value * 24)>   day;
        typedef const_int_type<(day::value * 7)>     week;
    }
    
    typedef class time_class time_class;
    
    typedef std::chrono::system_clock clock_type;
    typedef std::chrono::time_point<clock_type> time_point;
    
    struct tm current_time();
    const time_point& tmtotp( struct tm&, time_point&);
    const struct tm& tptotm(time_point&, struct tm&);
    int days_in_month(const int&, const int&);
    
    std::istream& operator>>(std::istream&, time_class&);
    std::ostream& operator<<(std::ostream&, const time_class&);
    
    /** Time: The indefinite continued progress of existence and events in the past, 
     * present, and future regarded as a whole. */
    typedef class time_class
    {
    public:
        explicit time_class(const struct tm&);
        explicit time_class();
        ~time_class();
        
        const struct tm& value() const;
        
        time_class& operator=(const time_class&);
        time_class& operator=(const struct tm&);
        
        //comparisons
        bool operator==(const time_class&) const;
        bool operator!=(const time_class&) const;
        bool operator<(const time_class&) const;
        bool operator>(const time_class&) const;
        bool operator<=(const time_class&) const;
        bool operator>=(const time_class&) const;
        
        //mathematical ops: 
        const time_class& operator++();
        time_class        operator+(int) const;
        const time_class& operator+=(int);
        const time_class& operator--();
        time_class        operator-(int) const;
        const time_class& operator-=(int);
        
        /* data retrieval.  I hate to do this, but the information
         * must be modified before reads and after writes. */
        
        int& second();//0-59
        const int& second() const;
        int& mhour(); //0-23
        const int& mhour() const;
        int& minute();//0-59
        const int& minute() const;
        int& month(); //0-11 
        const int& month() const;
        int& wday();  //0-6
        const int& wday() const;
        int& mday(); //1-31
        const int& mday() const;
        int& yday();
        const int& yday() const;
        int hour() const;
        bool am() const;
        int gyear() const;
        void syear(const int&);
        void smonth(const int&);
        std::string month_name() const;
        std::string wday_name() const;
        
        friend std::istream& operator>>(std::istream&, time_class&);
        friend std::ostream& operator<<(std::ostream&, const time_class&);
        
    private:
        struct tm cur_time;
        
        void sync_to_yday();
        void subtract_day();
        void add_day();
        
    } time_class;
    
}

#endif