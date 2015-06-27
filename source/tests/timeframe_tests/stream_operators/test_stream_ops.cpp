#ifdef UNIT_TEST_PROG

#include <stdexcept>
#include <exception>
#include <utility>

#include "test_stream_ops.hpp"
#include "utility/time_frame.hpp"
#include "tests/common_test_util.hpp"

namespace
{
    tdata::timeframe_class random_timeframe();
    tdata::time_interval_type random_interval();
    
    
    
    tdata::time_interval_type random_interval()
    {
        using test::common::random_number;
        
        tdata::time_interval_type interval;
        interval.u = (tdata::time_interval_type::unit_t)random_number(0, (tdata::time_interval_type::max_unit_t - 1));
        
        switch(interval.u)
        {
            case tdata::time_interval_type::unit_t::day:
            {
                interval.count = std::move(random_number(0, 1000));
            }
            break;
            
            case tdata::time_interval_type::unit_t::week:
            {
                interval.count = std::move(random_number(0, 150));
            }
            break;
            
            case tdata::time_interval_type::unit_t::month:
            {
                interval.count = std::move(random_number(0, 24));
            }
            break;
            
            case tdata::time_interval_type::unit_t::year:
            {
                interval.count = std::move(random_number(0, 10));
            }
            break;
            
            default:
            {
                throw std::runtime_error{"\n\nError, test invalid!\nInvalid time_interval_type::unit_t value produced!"};
            }
            break;
        }
        return interval;
    }
    
    tdata::timeframe_class random_timeframe()
    {
        constexpr unsigned int interval_begin_range{50};
        
        tdata::timeframe_class frame{std::move(random_interval())};
        switch(test::common::random_number(0, 1))
        {
            case 0:
            {
                frame.beg += test::common::random_number(0, (interval_begin_range * tdata::t_const::day));
            }
            break;
            
            case 1:
            {
                frame.beg -= test::common::random_number(0, (interval_begin_range * tdata::t_const::day));
            }
            break;
            
            default:
            {
                throw std::runtime_error{"\n\nError, test invalid!\nRandom number between \
0 and 1 (inclusive) produced a result outside of that range!"};
            }
            break;
        }
        return frame;
    }
    
    
}

namespace testing
{
    bool test_interval_streams()
    {
        using test::common::read_from_stream;
        
        tdata::timeframe_class f{std::move(random_timeframe())};
        return (f == read_from_stream<tdata::timeframe_class>(f));
    }
    
    
}


#endif