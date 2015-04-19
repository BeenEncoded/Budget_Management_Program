#include "build_defines.hpp"
#if UNIT_TEST_PROG == true
#include <string>
#include <iostream>

#include "common/global/global_defines.hpp"
#include "common_function_tests.hpp"
#include "common/common.hpp"
#include "tests/common_test_util.hpp"
#include "utility/ansi.hpp"
#include "utility/user_input.hpp"

namespace
{
    std::string key_code_display(const keyboard::key_code_data&);
    
    
    inline std::string key_code_display(const keyboard::key_code_data& key)
    {
        std::string temps("{");
        
        for(std::size_t x(0); x < key.control_d.size(); ++x)
        {
            temps += std::to_string(key.control_d[x]);
            if(x < (key.control_d.size() - 1)) temps += ", ";
        }
        temps += "}";
        return temps;
    }
    
    
}

namespace test
{
    bool test_get_user_string()
    {
        using std::cout;
        using std::endl;
        using ::common::get_user_str;
        
        bool finished(false), canceled(false);
        std::string input;
        
        do
        {
            ::common::cls();
            cout<< std::string(7, '\n');
            cout<< "Enter somthing: ";
            cout.flush();
            
            canceled = !::common::get_user_str(input);
            
            ::common::cls();
            cout<< std::string(7, '\n')<< "Input = \""<< input<< "\""<< endl;
            if(canceled)
            {
                cout<< "Canceled"<< endl;
            }
            else
            {
                cout<< "Accepted"<< endl;
            }
            ::common::wait();
            finished = !::common::prompt_user("Another?");
        }while(!finished);
        return true;
    }
    
    void test_key_codes()
    {
        using std::cout;
        using std::endl;
        
        keyboard::key_code_data key;
        
        ::user_input::cl();
        do
        {
            ::common::cls();
            key = ::user_input::getch_funct();
            cout<< "Key = "<< key_code_display(key)<< endl;
            ::common::wait();
        }while(::common::prompt_user("Another?"));
    }
    
    
}

#endif