

#include "common.hpp"

namespace
{
    unsigned long long pow(const unsigned int&, const unsigned int&);
    
    
    inline unsigned long long pow(const unsigned int& a, const unsigned int& b)
    {
        if(b == 1) return a;
        else if(b == 0) return 1;
        
        unsigned long long newa(a);
        for(unsigned int x = 1; x < b; ++x) newa *= a;
        return newa;
    }
    
    
}

namespace common
{
    int digit(const unsigned int& num, const unsigned short& place)
    {
        return (((num % pow(10, place)) - (num % pow(10, (place - 1)))) / (10 ^ (place - 1)));
    }
    
    int digit_count(const unsigned int& num)
    {
        int count(0);
        unsigned int temp(num);
        while(temp >= 10)
        {
            temp /= 10;
            ++count;
        }
        return ++count;
    }
    
    
}