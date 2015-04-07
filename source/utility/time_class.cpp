#include <ctime>
#include <chrono>
#include <iostream>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "time_class.hpp"
#include "globals.hpp"

namespace
{
    std::vector<unsigned short> month_days(const unsigned int&);
    unsigned int total_days_in_m(const int&, const int&);
    std::vector<std::string> day_names();
    std::vector<std::string> month_names();
    int find_month_of_yday(const int&, const int&);
    
    
    inline std::vector<unsigned short> month_days(const unsigned int& year)
    {
        return std::vector<unsigned short>({
            31,
                    (__isleap(year) ? (unsigned short)29 : (unsigned short)28),
                    31,
                    30,
                    31,
                    30,
                    31,
                    31,
                    30,
                    31,
                    30,
                    31});
    }
    
    inline unsigned int total_days_in_m(const int& m, const int& year)
    {
        unsigned int days(0);
        
        for(int x = 0, month = 0; x < (m + 1); x++, (month = ((month + 1) % 12)))
        {
            days += month_days(year)[month];
        }
        return days;
    }
    
    inline std::vector<std::string> day_names()
    {
        return std::vector<std::string>({
            "Sunday",
                    "Monday",
                    "Tuesday",
                    "Wednesday",
                    "Thursday",
                    "Friday",
                    "Saturday"});
    }
    
    inline std::vector<std::string> month_names()
    {
        return std::vector<std::string>({
            "January",
                    "February",
                    "March",
                    "April",
                    "May",
                    "June",
                    "July",
                    "August",
                    "September",
                    "October",
                    "November",
                    "December"});
    }
    
    inline int find_month_of_yday(const int& day, const int& year)
    {
        int month(0);
        
        for(month = 0; ((month < 12) && ((signed)total_days_in_m(month, year) <= day)); month++);
        return month;
    }
    
    
}

/** Stream operators: */
namespace tdata
{
    const time_point& tmtotp(struct tm& t, time_point& tempt)
    {
        tempt = std::chrono::system_clock::from_time_t(mktime(&t));
        return tempt;
    }
    
    const struct tm& tptotm(time_point& t, struct tm& tempt)
    {
        time_t lvalue_timet(std::chrono::system_clock::to_time_t(t));
        tempt = (*localtime(&lvalue_timet));
        return tempt;
    }
    
    std::istream& operator>>(std::istream& in, time_class& t)
    {
        char *ch(new char[sizeof(struct tm)]);
        
        in.peek();
        if(in.good())
        {
            for(unsigned int x = 0; ((x < sizeof(struct tm)) && in.good()); x++) ch[x] = in.get();
            if(in.good())
            {
                std::memcpy(&(t.cur_time), ch, sizeof(struct tm));
            }
            in.peek();
        }
        delete[] ch;
        return in;
    }
    
    std::ostream& operator<<(std::ostream& out, const time_class& t)
    {
        char *ch(new char[sizeof(struct tm)]);
        if(out.good())
        {
            std::memcpy(ch, &(t.cur_time), sizeof(struct tm));
            for(unsigned int x = 0; x < sizeof(struct tm); x++) out<< ch[x];
        }
        delete[] ch;
        return out;
    }
    
    
}

/** Time_class member functions: */
namespace tdata
{
    time_class::time_class(const time_class& t) noexcept : 
            cur_time(t.cur_time)
    {
    }
    
    time_class::time_class(time_class&& t) noexcept : 
            cur_time(std::move(t.cur_time))
    {
    }
    
    time_class::time_class(const struct tm& t) : cur_time(t)
    {
    }
    
    time_class::time_class() : cur_time(current_time())
    {
    }
    
    time_class::~time_class()
    {
    }
    
    const struct tm& time_class::value() const
    {
        return this->cur_time;
    }
    
    time_class& time_class::operator=(const time_class& t)
    {
        if(this != &t)
        {
            std::memcpy(&(this->cur_time), &t, sizeof(t));
        }
        return *this;
    }
    
    time_class& time_class::operator=(time_class&& t) noexcept
    {
        this->cur_time = std::move(t.cur_time);
        return *this;
    }
    
    time_class& time_class::operator=(const struct tm& t)
    {
        if(&(this->cur_time) != (&t))
        {
            std::memcpy(&(this->cur_time), &t, sizeof(t));
        }
        return *this;
    }
    
    bool time_class::operator==(const time_class& t) const
    {
        return ((this->cur_time.tm_sec == t.cur_time.tm_sec) && 
                    (this->cur_time.tm_min == t.cur_time.tm_min) && 
                    (this->cur_time.tm_hour == t.cur_time.tm_hour) && 
                    (this->cur_time.tm_mday == t.cur_time.tm_mday) && 
                    (this->cur_time.tm_wday == t.cur_time.tm_wday) && 
                    (this->cur_time.tm_yday == t.cur_time.tm_yday) && 
                    (this->cur_time.tm_mon == t.cur_time.tm_mon) && 
                    (this->cur_time.tm_year == t.cur_time.tm_year) && 
                    (std::strcmp(this->cur_time.tm_zone, t.cur_time.tm_zone) == 0) && 
                    (this->cur_time.tm_isdst == t.cur_time.tm_isdst));
    }
    
    bool time_class::operator!=(const time_class& t) const
    {
        return ((this->cur_time.tm_sec != t.cur_time.tm_sec) && 
                    (this->cur_time.tm_min != t.cur_time.tm_min) && 
                    (this->cur_time.tm_hour != t.cur_time.tm_hour) && 
                    (this->cur_time.tm_mday != t.cur_time.tm_mday) && 
                    (this->cur_time.tm_wday != t.cur_time.tm_wday) && 
                    (this->cur_time.tm_yday != t.cur_time.tm_yday) && 
                    (this->cur_time.tm_mon != t.cur_time.tm_mon) && 
                    (this->cur_time.tm_year != t.cur_time.tm_year) && 
                    (std::strcmp(this->cur_time.tm_zone, t.cur_time.tm_zone) != 0) && 
                    (this->cur_time.tm_isdst != t.cur_time.tm_isdst));
    }
    
    bool time_class::operator<(const time_class& t) const
    {
        if(this->cur_time.tm_year < t.cur_time.tm_year) return true;
        else if(this->cur_time.tm_year == t.cur_time.tm_year)
        {
            if(this->cur_time.tm_yday < t.cur_time.tm_yday) return true;
            else if(this->cur_time.tm_yday == t.cur_time.tm_yday)
            {
                if(this->cur_time.tm_hour < t.cur_time.tm_hour) return true;
                else if(this->cur_time.tm_hour == t.cur_time.tm_hour)
                {
                    if(this->cur_time.tm_min < t.cur_time.tm_min) return true;
                    else if(this->cur_time.tm_min == t.cur_time.tm_min)
                    {
                        if(this->cur_time.tm_sec < t.cur_time.tm_sec) return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool time_class::operator<=(const time_class& t) const
    {
        return (this->operator<(t) || this->operator==(t));
    }
    
    bool time_class::operator>(const time_class& t) const
    {
        return !this->operator<=(t);
    }
    
    bool time_class::operator>=(const time_class& t) const
    {
        return !(this->operator<(t));
    }
    
    /** Adds 1 second to the time. */
    const time_class& time_class::operator++()
    {
        this->cur_time.tm_sec++;
        if(this->cur_time.tm_sec > 59)
        {
            this->cur_time.tm_sec = 0;
            this->cur_time.tm_min++;
            if(this->cur_time.tm_min > 59)
            {
                this->cur_time.tm_min = 0;
                this->cur_time.tm_hour++;
                if(this->cur_time.tm_hour > 23)
                {
                    this->cur_time.tm_hour = 0;
                    this->add_day();
                }
            }
        }
        return *this;
    }
    
    /** Adds (i) seconds to the time.  Optimizations are in place 
     *  so that if (i) represents a value that can be calculated in
     * days, there won't be any significant CPU Overhead. */
    time_class time_class::operator+(int i) const
    {
        using namespace t_const;
        
        time_class tempt(*this);
        
        if(i >= (signed)day::value)
        {
            for(int x = 0; x < (i / (signed)day::value); x++) tempt.add_day();
            i %= day::value;
        }
        
        if(i < 0) for(int x = 0; x < (i * (-1)); x++) --tempt;
        else if(i > 0) for(int x = 0; x < i; x++) ++tempt;
        return tempt;
    }
    
    const time_class& time_class::operator+=(int i)
    {
        (*this) = ((*this) + i);
        return *this;
    }
    
    const time_class& time_class::operator--()
    {
        if(this->cur_time.tm_sec == 0)
        {
            this->cur_time.tm_sec = 59;
            if(this->cur_time.tm_min == 0)
            {
                this->cur_time.tm_min = 59;
                if(this->cur_time.tm_hour == 0)
                {
                    this->cur_time.tm_hour = 23;
                    this->subtract_day();
                }
                else if(this->cur_time.tm_hour > 0) this->cur_time.tm_hour--;
            }
            else if(this->cur_time.tm_min > 0) this->cur_time.tm_min--;
        }
        else if(this->cur_time.tm_sec > 0) this->cur_time.tm_sec--;
        return *this;
    }
    
    /** Subtracts (i) seconds to the time.  Optimizations are in place 
     *  so that if (i) represents a value that can be calculated in
     * days, there won't be any significant CPU Overhead. */
    time_class time_class::operator-(int i) const
    {
        using namespace t_const;
        
        time_class tempt(*this);
        
        if(i >= (signed)day::value)
        {
            for(int x = 0; x < (i / (signed)day::value); x++) tempt.subtract_day();
            i %= day::value;
        }
        
        if(i < 0) for(int x = 0; x < (i * (-1)); x++) ++tempt;
        else if(i > 0) for(int x = 0; x < i; x++) --tempt;
        return tempt;
    }
    
    const time_class& time_class::operator-=(int i)
    {
        (*this) = ((*this) - i);
        return *this;
    }
    
    int& time_class::second()
    {
        return this->cur_time.tm_sec;
    }
    
    int& time_class::minute()
    {
        return this->cur_time.tm_min;
    }
    
    /* Hour in military format */
    int& time_class::mhour()
    {
        return this->cur_time.tm_hour;
    }
    
    int& time_class::month()
    {
        return this->cur_time.tm_mon;
    }
    
    int& time_class::wday()
    {
        return this->cur_time.tm_wday;
    }
    
    /* Hour in 12-hour format */
    int time_class::hour() const
    {
        int tempi(this->cur_time.tm_hour % 12);
        if(tempi == 0) tempi = 12;
        return tempi;
    }
    
    bool time_class::am() const
    {
        return (this->cur_time.tm_hour < 12);
    }
    
    int time_class::gyear() const
    {
        return (this->cur_time.tm_year + 1900);
    }
    
    void time_class::syear(const int& i)
    {
        using namespace t_const;
        
        int tempyday(this->cur_time.tm_yday), target(i - 1900);
        
        if(__isleap((this->cur_time.tm_year + 1900)) && (tempyday == 365))
        {
            tempyday--;
            (*this) = this->operator-(day::value);
        }
        if(target < this->cur_time.tm_year)
        {
            while(target < this->cur_time.tm_year) (*this) = this->operator-(day::value);
        }
        else if(target > this->cur_time.tm_year)
        {
            while(target > this->cur_time.tm_year) (*this) = this->operator+(day::value);
        }
        while(this->cur_time.tm_yday != 0) (*this) = this->operator-(day::value);
        while(this->cur_time.tm_yday != tempyday) (*this) = this->operator+(day::value);
    }
    
    /* Syncs the month and month day to the day of the year.  This allows
     * us to simply do math with yday, and then "sync" the month and day of 
     * month to the yday. */
    void time_class::sync_to_yday()
    {
        this->cur_time.tm_mon = find_month_of_yday(this->cur_time.tm_yday, (this->cur_time.tm_year + 1900));
        if(this->cur_time.tm_mon == 0)
        {
            this->cur_time.tm_mday = (this->cur_time.tm_yday + 1);
        }
        else
        {
            this->cur_time.tm_mday = ((this->cur_time.tm_yday + 1) - total_days_in_m((this->cur_time.tm_mon - 1), (this->cur_time.tm_year + 1900)));
        }
    }
    
    std::string time_class::month_name() const
    {
        return month_names().at(this->cur_time.tm_mon);
    }
    
    std::string time_class::wday_name() const
    {
        return day_names().at(this->cur_time.tm_wday);
    }
    
    const int& time_class::second() const
    {
        return this->cur_time.tm_sec;
    }
    
    const int& time_class::mhour() const
    {
        return this->cur_time.tm_hour;
    }
    
    const int& time_class::minute() const
    {
        return this->cur_time.tm_min;
    }
    
    const int& time_class::month() const
    {
        return this->cur_time.tm_mon;
    }
    
    const int& time_class::wday() const
    {
        return this->cur_time.tm_wday;
    }
    
    int& time_class::mday()
    {
        return this->cur_time.tm_mday;
    }
    
    const int& time_class::mday() const
    {
        return this->cur_time.tm_mday;
    }
    
    void time_class::subtract_day()
    {
        if(this->cur_time.tm_wday == 0) this->cur_time.tm_wday = 6;
        else this->cur_time.tm_wday--;
        
        if(this->cur_time.tm_yday == 0)
        {
            this->cur_time.tm_year--;
            this->cur_time.tm_yday = (__isleap((this->cur_time.tm_year + 1900)) ? 365 : 364);
        }
        else if(this->cur_time.tm_yday > 0) this->cur_time.tm_yday--;
        this->sync_to_yday();
    }
    
    void time_class::add_day()
    {
        this->cur_time.tm_wday++;
        this->cur_time.tm_wday %= 7;
        this->cur_time.tm_yday++;
        if(this->cur_time.tm_yday > (__isleap((this->cur_time.tm_year + 1900)) ? 365 : 364))
        {
            this->cur_time.tm_yday = 0;
            this->cur_time.tm_year++;
        }
        this->sync_to_yday();
    }
    
    int& time_class::yday()
    {
        return this->cur_time.tm_yday;
    }
    
    const int& time_class::yday() const
    {
        return this->cur_time.tm_yday;
    }
    
    void time_class::smonth(const int& x)
    {
        int tempi(x);
        
        auto add_month = [](time_class& t)->void
        {
            using namespace tdata::t_const;
            using tdata::days_in_month;
            
            int mday(t.mday());
            
            if(mday > days_in_month(((t.month() + 1) % 12), t.gyear()))
            {
                mday %= days_in_month(((t.month() + 1) % 12), t.gyear());
            }
            if(t.mday() == 1) t += day::value;
            while(t.mday() != 1) t += day::value;
            while(t.mday() != mday) t += day::value;
        };
        
        auto subtract_month = [](time_class& t)->void
        {
            using namespace tdata::t_const;
            using tdata::days_in_month;
            
            int mday(t.mday());
            
            if(mday > days_in_month(((t.month() + 11) % 12), t.gyear()))
            {
                mday %= days_in_month(((t.month() + 11) % 12), t.gyear());
            }
            while(t.mday() != 1) t -= day::value;
            t -= day::value;
            while(t.mday() != mday) t -= day::value;
        };
        if(tempi < 0)
        {
            this->syear(this->gyear() - 1);
            tempi = 11;
        }
        else if(tempi > 11)
        {
            this->syear(this->gyear() + 1);
            tempi = 0;
        }
        while(this->cur_time.tm_mon != 0) subtract_month(*this);
        while(this->cur_time.tm_mon != tempi) add_month(*this);
    }
    
    
}

/** misc functions: */
namespace tdata
{
    struct tm current_time()
    {
        time_point now(std::chrono::system_clock::now());
        struct tm temptm;
        tptotm(now, temptm);
        return temptm;
    }
    
    int days_in_month(const int& month, const int& year)
    {
        if((month < 0) || (month > 11)) return 0;
        return month_days(year).at(month);
    }
    
    
}

