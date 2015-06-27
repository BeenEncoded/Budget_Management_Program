#include <utility>
#include <exception>
#include <iostream>
#include <string>
#include <stdexcept>

#include "time_frame.hpp"
#include "utility/time_class.hpp"
#include "utility/stream_operations.hpp"


#define CODE_LOCATION std::string{__FILE__ + std::string{"; Line "} + std::to_string(__LINE__) + \
":  "}

namespace
{
    void set_month_day(tdata::time_class&, const int&);
    
    
    
    /**
     * @brief Sets the month_day of a time to the specified mday value.
     * @param mday The day value to set it to.  Not garunteed to be
     * equal to the result if the argument passed is out of range.
     */
    inline void set_month_day(tdata::time_class& t, const int& m) //todo make this part of time_class
    {
        constexpr const char *const funct_name{"void set_month_day(tdata::time_class&, const int&);"};
        
        int tempi{t.month()}, mday{m};
        
        if(mday < 0) mday = 1;
        
        //reset month day to 1:
        if(t.mday() != 1)
        {
            for(unsigned int x{0}; ((x < 32) && (t.mday() > 1)); ++x) t -= tdata::t_const::day;
            
            /* If, for some inexplicable reason, there is an issue with the time_class
             * that causes this algorithm to fail, this redundancy should catch it.
             * THIS IS NOT A REDUNDANCY, after this, the t.mday() should be 
             * garunteed to be 1.  The month must also remained unchanged as well.*/
            if((t.mday() != 1) || (t.month() != tempi))
            {
                throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nERROR in " + 
                        std::string{funct_name} + ":\ntdata::time_class::mday \
algorithm could not reset the month day value to 1; mathematical error!"};
            }
        }
        
        //now set the month day equal to the closest possible value:
        if(t.mday() != mday)
        {
            if(tdata::days_in_month(t.month(), t.gyear()) < mday) mday = std::move(tdata::days_in_month(t.month(), t.gyear()));
            while((t.mday() != mday) && (t.month() == tempi)) t += tdata::t_const::day;
            if(t.month() != tempi) t -= tdata::t_const::day;
        }
        
        //redundancy
        if((t.month() != tempi) || (t.mday() != mday))
        {
            throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nError in " + 
                    std::string{funct_name} + ": Failed to add a month and keep \
original month_day value!"};
        }
    }
    
    
}

//time_interval_type
namespace tdata
{
    time_interval_type::time_interval_type() : 
            u{day},
            count{0}
    {
    }
    
    time_interval_type::time_interval_type(const unsigned int& c, const unit_t& un) : 
            u{un},
            count{c}
    {
        if((unsigned int)this->u >= time_interval_type::max_unit_t)
        {
            throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nError in \
time_interval_type::time_interval_type(const unsigned int& c, const unit_t& un):\n\
Constructor called with invalid unit_type!"};
        }
    }
    
    time_interval_type::time_interval_type(const time_interval_type& t) : 
            u{t.u},
            count{t.count}
    {
    }
    
    time_interval_type::time_interval_type(time_interval_type&& t) noexcept :
            u{std::move(t.u)},
            count{std::move(t.count)}
    {
    }
    
    time_interval_type::~time_interval_type()
    {
    }
    
    time_interval_type& time_interval_type::operator=(const time_interval_type& t)
    {
        if(this != &t)
        {
            this->u = t.u;
            this->count = t.count;
        }
        return *this;
    }
    
    time_interval_type& time_interval_type::operator=(time_interval_type&& t) noexcept
    {
        if(this != &t)
        {
            this->u = std::move(t.u);
            this->count = std::move(t.u);
        }
        return *this;
    }
    
    bool time_interval_type::operator==(const time_interval_type& t) const
    {
        return ((this->count == t.count) && 
                (this->u == t.u));
    }
    
    bool time_interval_type::operator!=(const time_interval_type& t) const
    {
        return ((this->count != t.count) || 
                (this->u != t.u));
    }
    
    /**
     * @return The name of the unit of time this interval represents.  This is provided
     * mainly for user interface or display purposes.
     */
    std::string time_interval_type::unit_name() const
    {
        switch(this->u)
        {
            case unit_t::day: return "day";
            case unit_t::week: return "week";
            case unit_t::month: return "month";
            case unit_t::year: return "year";
            
            default: break;
        }
        throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nIn function \
std::string time_interval_type::unit_name() const:\n Unit type does not exist!!"};
        return "";
    }
    
    /**
     * @brief Adds one month to a time_class.  Attempts to change only the month.
     */
    void time_interval_type::add_month(tdata::time_class& t) const
    {
        int tempi{t.month()}, mday{t.mday()};
        
        for(unsigned int x{0}; ((x < 32) && (tempi == t.month())); ++x) t += tdata::t_const::day;
        
        /*if we can't garuntee that the month has indeed been incremented,
         * then this algorithm has failed. */
        if(t.month() != ((tempi + 1) % 12))
        {
            throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nError in \
void time_interval_type::add_month(tdata::time_class&) const:\nMonth value remains \
the same after addition!  Mathematical error, algorithm failed."};
        }
        set_month_day(t, mday);
    }
    
    /**
     * @brief Subtracts one month from a time_class.  Attempts to change only the month.
     */
    void time_interval_type::sub_month(tdata::time_class& t) const
    {
        constexpr const char *const funct_name{"void time_interval_type::sub_month(tdata::time_class&) const"};
        
        int tempi{t.month()}, mday{t.mday()};
        
        for(unsigned int x{0}; ((x < 32) && (tempi == t.month())); ++x) t -= tdata::t_const::day;
        
        if(t.month() != ((tempi + 11) % 12))
        {
            throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nError in " + 
                    std::string{funct_name} + ":\nThe value of the month was not subracted from, \
Algorithm failed!"};
        }
        set_month_day(t, mday);
    }
    
    /**
     * @brief Adds one year to the time class.
     */
    void time_interval_type::add_year(tdata::time_class& t) const
    {
        t.syear(t.gyear() + 1);
    }
    
    /**
     * @brief Subtracts one year from the time class.
     */
    void time_interval_type::sub_year(tdata::time_class& t) const
    {
        if(t.gyear() > 1)
        {
            t.syear(t.gyear() - 1);
        }
    }
    
    /**
     * @brief Adds this interval to the time_class passed and returns the result.
     * this can be used to determine the end_date of an interval.
     * @return The result.
     */
    tdata::time_class time_interval_type::add_to(const tdata::time_class& t) const
    {
        return this->op_on(t, false);
    }
    
    /**
     * @brief Performs either addition or subtraction on the time_class passed
     * based on the value of 'sub'.  If 'sub' is true, it will subtract from the
     * time_class.
     * @return The result of the operation.
     */
    tdata::time_class time_interval_type::op_on(const tdata::time_class& t, const bool& sub) const
    {
        if(this->count == 0) return time_class{t};
        
        constexpr const char *const funct_name{"tdata::time_class time_interval_type::op_on(const tdata::time_class&, const bool&) const"};
        
        tdata::time_class end_time{t};
        long temp_length{0};
        
        switch(this->u)
        {
            case unit_t::day:
            if(temp_length == 0) temp_length = t_const::day;
            case unit_t::week:
            if(temp_length == 0) temp_length = t_const::week;
            for(decltype(this->count) x{0}; x < this->count; ++x)
            {
                if(sub) end_time += temp_length;
                else end_time -= temp_length;
            }
            break;
            
            case unit_t::month:
            {
                for(decltype(this->count) x{0}; x < this->count; ++x)
                {
                    if(sub) this->add_month(end_time);
                    else this->sub_month(end_time);
                }
            }
            break;
            
            case unit_t::year:
            {
                for(decltype(this->count) x{0}; x < this->count; ++x)
                {
                    if(sub) this->add_year(end_time);
                    else this->sub_year(end_time);
                }
            }
            break;
            
            default:
            {
                throw std::runtime_error{"\n\n" + CODE_LOCATION + "\nError in " + 
                        std::string{funct_name} + ":\nInvalid \
tdata::time_interval_type::unit_t value passed!"};
            }
            break;
        }
        return end_time;
    }
    
    
}

//timeframe_class
namespace tdata
{
    timeframe_class::timeframe_class() : 
            beg{current_time()},
            interval{}
    {
    }
    
    timeframe_class::timeframe_class(const time_class& t, const time_interval_type& i) : 
            beg{t},
            interval{i}
    {
    }
    
    timeframe_class::timeframe_class(const unsigned int& d, const time_interval_type::unit_t& u) : 
            beg{current_time()},
            interval{d, u}
    {
    }
    
    timeframe_class::timeframe_class(const time_interval_type& t) : 
            beg{current_time()},
            interval{t}
    {
    }
    
    timeframe_class::timeframe_class(const timeframe_class& t) : 
            beg{t.beg},
            interval{t.interval}
    {
    }
    
    timeframe_class::timeframe_class(timeframe_class&& t) noexcept : 
            beg{std::move(t.beg)},
            interval{std::move(t.interval)}
    {
    }
    
    timeframe_class::~timeframe_class()
    {
    }
    
    timeframe_class& timeframe_class::operator=(const timeframe_class& t)
    {
        if(this != &t)
        {
            this->beg = t.beg;
            this->interval = t.interval;
        }
        return *this;
    }
    
    timeframe_class& timeframe_class::operator=(timeframe_class&& t) noexcept
    {
        if(this != &t)
        {
            this->beg = std::move(t.beg);
            this->interval = std::move(t.interval);
        }
        return *this;
    }
    
    bool timeframe_class::operator==(const timeframe_class& t) const
    {
        return ((this->beg == t.beg) && 
                (this->interval == t.interval));
    }
    
    bool timeframe_class::operator!=(const timeframe_class& t) const
    {
        return ((this->beg != t.beg) || 
                (this->interval != t.interval));
    }
    
    /**
     * @brief returns true if the time given is within 
     */
    bool timeframe_class::is_within_frame(const time_class& t) const
    {
        if(this->interval.count == 0) return false;
        return ((t > this->beg) && (this->interval.add_to(beg) > t));
    }
    
    /**
     * @brief Returns true if two timeframe_class objects overlap.
     */
    bool timeframe_class::overlaps(const timeframe_class& t) const
    {
        if(this->interval.count == 0) return false;
        return (this->is_within_frame(t.beg) ||
                this->is_within_frame(t.interval.add_to(t.beg)));
    }
    
    const time_interval_type& timeframe_class::cinterval() const noexcept
    {
        return this->interval;
    }
    
    
}

//stream operators
namespace tdata
{
    //time_interval_type
    std::istream& operator>>(std::istream& in, time_interval_type& t)
    {
        using namespace utility;
        
        t = std::move(time_interval_type{});
        in.peek();
        if(in.good())
        {
            unsigned int tempi{(unsigned int)t.u};
            in_mem<unsigned int>(in, tempi);
            t.u = (tdata::time_interval_type::unit_t)tempi;
        }
        if(in.good()) in_mem<decltype(t.count)>(in, t.count);
        return in;
    }
    
    std::ostream& operator<<(std::ostream& out, const time_interval_type& t)
    {
        if(out.good())
        {
            using namespace utility;
            out_mem<unsigned int>(out, (unsigned int)t.u);
            out_mem<decltype(t.count)>(out, t.count);
        }
        return out;
    }
    
    //timeframe_class
    std::istream& operator>>(std::istream& in, timeframe_class& t)
    {
        t = std::move(timeframe_class{});
        in.peek();
        if(in.good()) in>> t.beg;
        if(in.good()) in>> t.interval;
        return in;
    }
    
    std::ostream& operator<<(std::ostream& out, const timeframe_class& t)
    {
        if(out.good())
        {
            out<< t.beg;
            out<< t.interval;
        }
        return out;
    }
    
    
}

