#if UNIT_TEST_PROG == true

#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <sstream>

#include "stream_ops.hpp"
#include "common/global/global_defines.hpp"
#include "tests/common_test_util.hpp"
#include "data/budget_data.hpp"
#include "utility/time_class.hpp"


namespace
{
    data::budget_data random_budget();
    std::vector<data::money_alloc_data> random_alloc_list(const unsigned int&, const unsigned int&);
    std::string alloc_display(const data::money_alloc_data&);
    std::string budget_display(const data::budget_data&);
    std::string time_to_string(const tdata::time_class&);
    std::vector<data::budget_data> random_budget_list(const unsigned int&, const unsigned int&);
    void display_failure(const data::budget_data&, const data::budget_data&);
    template<typename type> std::vector<type> read_vect_from_stream(const std::vector<type>&);
    std::string budget_vec_display(const std::vector<data::budget_data>&);
    
    
    
    inline std::vector<data::money_alloc_data> random_alloc_list(const unsigned int& min, const unsigned int& max)
    {
        using test::common::random_number;
        using test::common::random_string;
        
        unsigned int size(random_number(min, max));
        std::vector<data::money_alloc_data> allocs;
        
        for(unsigned int x(0); x < size; ++x)
        {
            allocs.emplace_back(random_string((std::string(LETTERS) + " "), 5, 30), random_number(10, 500));
            allocs.back().id = std::move(data::new_alloc_id(allocs));
        }
        return allocs;
    }
    
    inline data::budget_data random_budget()
    {
        using test::common::random_number;
        
        data::budget_data bud((data::money_t)random_number(100, 10000));
        
        bud.allocs = std::move(random_alloc_list(3, 10));
        return bud;
    }
    
    /**
     * @brief Converts a time_class to a string for display purposes.
     * @param t the time.
     * @return a string that represents the time_class passed.
     */
    inline std::string time_to_string(const tdata::time_class& t)
    {
        return (std::to_string(t.hour())  + ":" + std::to_string(t.minute()) + std::string(" ") + 
                (t.am() ? "AM" : "PM") + std::string("  |  (") + t.wday_name() + ") " + 
                t.month_name() + " " + std::to_string(t.mday()) + ", " + std::to_string(t.gyear()));
    }
    
    /**
     * @brief Returns a human-readable string that represents a money_alloc_data
     * mainly for display and debugging purposes.
     * @param allocation the alloc to use.
     * @return a string that can be used for debugging.
     */
    inline std::string alloc_display(const data::money_alloc_data& allocation)
    {
        return ("[\"" + allocation.name + "\" : " + std::to_string(allocation.value) + 
                " :  " + std::to_string(allocation.id) + "]");
    }
    
    /**
     * @brief Returns a human-readable string that can be used for debugging.
     * @param b the budget_data to use.
     * @return a human-readble string that represents a budget_data.
     */
    inline std::string budget_display(const data::budget_data& b)
    {
        std::string disp("[$" + std::to_string(b.total_money) + " :: " + time_to_string(b.timeframe.beg) + " :: ");
                
        disp += "\n\t{\n";
        for(unsigned int x(0); x < b.allocs.size(); ++x) disp += ("\t\t" + alloc_display(b.allocs[x]) + "\n");
        disp += "\t}\n]";
        return disp;
    }
    
    inline std::vector<data::budget_data> random_budget_list(const unsigned int& min, const unsigned int& max)
    {
        using test::common::random_number;
        
        std::vector<data::budget_data> budgets;
        std::size_t size(random_number(min, max));
        
        for(unsigned int x(0); x < size; ++x)
        {
            budgets.push_back(std::move(random_budget()));
        }
        return budgets;
    }
    
    /**
     * @brief Displays the data for a failed read/write test on a budget_data.
     * @param w budget_data written to the stream.
     * @param r budget_data read from the stream.
     */
    inline void display_failure(const data::budget_data& w, const data::budget_data& r)
    {
        using std::cout;
        using std::endl;
        
        cout<< std::string(70, '-')<< endl<< endl<< budget_display(w)<< endl<< endl<< 
                "Read as: "<< endl<< endl<< budget_display(r)<< endl<< endl<<
                std::string(70, '-')<< endl<< endl;
    }
    
    template<typename type>
    std::vector<type> read_vect_from_stream(const std::vector<type>& t)
    {
        std::stringstream ss;
        std::vector<type> stuff;
        
        for(typename std::vector<type>::const_iterator it(t.begin()); it != t.end(); ++it)
        {
            ss<< *it;
        }
        while(ss.good())
        {
            stuff.emplace_back();
            ss>> stuff.back();
        }
        return stuff;
    }
    
    inline std::string budget_vec_display(const std::vector<data::budget_data>& b)
    {
        std::string temps(70, 'V');
        
        temps += "\n";
        {
            unsigned int x(0);
            for(std::vector<data::budget_data>::const_iterator it(b.begin()); it != b.end(); ++it)
            {
                temps += ("Element " + std::to_string(++x) + ":\n");
                temps += (budget_display(*it) + "\n\n");
            }
        }
        temps += (std::string(70, '^') + "\n");
        return temps;
    }
    
    
}

namespace test
{
    bool test_budget_data_streams()
    {
        //namespaces & namespace objects:
        using data::budget_data;
        using std::vector;
        using test::common::read_from_stream;
        using std::cout;
        using std::endl;
        
        //variables:
        vector<budget_data> data(std::move(random_budget_list(5, 15)));
        unsigned int fails(0);
        bool success(true);
        
        for(vector<budget_data>::const_iterator it(data.begin()); it != data.end(); ++it)
        {
            if(read_from_stream<budget_data>(*it) != *it)
            {
                success = false;
                ++fails;
                display_failure(*it, read_from_stream<budget_data>(*it));
            }
        }
        if(fails > 0)
        {
            cout<< "Failure, the budgets listed failed the test.  ";
            if(fails == data.size()) cout<< "No budgets passed!";
            else cout<< "Failures: "<< std::to_string(fails)<< " / "<< std::to_string(data.size());
            cout<< endl<< endl<< endl;
        }
        cout.flush();
        return success;
    }
    
    bool test_multiple_budget_streams()
    {
        using test::common::read_from_stream;
        using std::cout;
        using std::endl;
        
        std::vector<data::budget_data> data(std::move(random_budget_list(5, 15))), 
                result(std::move(read_from_stream(data)));
        
        if(result != data)
        {
            cout<< "data: "<< endl;
            cout<< budget_vec_display(data)<< endl<< endl;
            cout<< "Result: "<< endl;
            cout<< budget_vec_display(result)<< endl<< endl;
        }
        
        return (result == data);
    }
    
    bool test_empty_vec()
    {
        std::vector<data::budget_data> data;
        for(unsigned int x(0); x < 10; ++x) data.push_back(data::budget_data());
        
        return (test::common::read_from_stream(data) == data);
    }
    
    
}


#endif