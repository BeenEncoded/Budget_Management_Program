#ifdef UNIT_TEST_PROG
#include <string>
#include <iostream>

#include "tests/common_test_util.hpp"
#include "utility/time_frame.hpp"
#include "utility/time_class.hpp"
#include "overlap_test.hpp"

namespace
{
    tdata::timeframe_class random_timeframe();
    tdata::time_interval_type random_interval(const unsigned int&, const unsigned int&);
    tdata::time_class random_time();
    std::string time_display(const tdata::time_class&);
    std::string date_display(const tdata::time_class&);
    tdata::time_class date_object(const int&, const int&, const int&);
    
    
    /**
     * @return a random time_interval_type that is common::random_number(min, max) units long.
     * The unit used is randomly chosen.
     */
    tdata::time_interval_type random_interval(const unsigned int& min, const unsigned int& max)
    {
        using namespace test;
        
        return tdata::time_interval_type{common::random_number(min, max), 
                (tdata::time_interval_type::unit_t)common::random_number(0, (tdata::time_interval_type::max_unit_t - 1))};
    }
    
    tdata::time_class random_time()
    {
        using namespace test;
        
        constexpr unsigned int min{5}, max{50};
        
        tdata::time_class temp_time{tdata::current_time()};
        bool sub{(bool)common::random_number(0, 1)};
        decltype(tdata::t_const::day) days{(common::random_number(min, max) * tdata::t_const::day)};
        
        if(sub) temp_time -= days;
        else temp_time += days;
        return temp_time;
    }
    
    __attribute__((unused)) tdata::timeframe_class random_timeframe()
    {
        return tdata::timeframe_class{random_time(), random_interval(7, (7 * 3))};
    }
    
    tdata::time_class date_object(const int& month, const int& day, const int& year)
    {
        tdata::time_class temp_time{tdata::current_time()};
        
        //set the year, then set to first day of that year:
        while(temp_time.gyear() > year) temp_time -= tdata::t_const::day;
        while(temp_time.gyear() < year) temp_time += tdata::t_const::day;
        while(temp_time.yday() > 0) temp_time -= tdata::t_const::day;
        
        //now set the month:
        while(temp_time.month() < (month - 1)) temp_time += tdata::t_const::day;
        //set the day:
        while((temp_time.mday() < day) && (temp_time.month() == (month - 1))) temp_time += tdata::t_const::day;
        if(temp_time.month() > (month - 1)) temp_time -= tdata::t_const::day;
        return temp_time;
    }
    
    __attribute__((unused)) std::string time_display(const tdata::time_class& t)
    {
        return (std::to_string(t.hour()) + ":" + std::to_string(t.minute()) + 
                (t.am() ? " am" : " pm"));
    }
    
    std::string date_display(const tdata::time_class& t)
    {
        return (t.month_name() + " " + std::to_string(t.mday()) + ", " + std::to_string(t.gyear()));
    }
    
    std::string frame_display(const tdata::timeframe_class& frame)
    {
        std::string temps{"Date begins:" + date_display(frame.beg)};
        temps += ("\nDate ends: " + date_display(frame.cinterval().add_to(frame.beg)));
        return temps;
    }
    
    
}

namespace testing
{
    bool overlap_is_good()
    {
        using std::cout;
        using std::endl;
        
        tdata::time_interval_type interval{1, tdata::time_interval_type::unit_t::week};
        tdata::timeframe_class frame{date_object(7, 1, 2015), interval};
        cout<< "1-week interval:"<< endl;
        cout<< frame_display(frame)<< endl<< endl;
        
        interval = std::move(tdata::time_interval_type{1, tdata::time_interval_type::unit_t::month});
        frame = std::move(tdata::timeframe_class{date_object(7, 1, 2015), interval});
        cout<< "1 month interval:"<< endl;
        cout<< frame_display(frame)<< endl<< endl;
        
        interval = std::move(tdata::time_interval_type{1, tdata::time_interval_type::unit_t::day});
        frame = std::move(tdata::timeframe_class{date_object(7, 1, 2015), interval});
        cout<< "1 day interval:"<< endl;
        cout<< frame_display(frame)<< endl;
        return true;
    }
    
    
}


#endif