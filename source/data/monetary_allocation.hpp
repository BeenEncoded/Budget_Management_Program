#ifndef MONETARY_ALLOCATION_HPP_INCLUDED
#define MONETARY_ALLOCATION_HPP_INCLUDED
#include <utility>
#include <vector>
#include <string>

#include "utility/time_class.hpp"

namespace data
{
    typedef long double money_t;
    typedef unsigned short priority_t;
    
    typedef class budget_class budget_class;
    typedef struct money_alloc_data money_alloc_data;
    
    
    typedef struct money_alloc_data
    {
        explicit money_alloc_data();               //todo
        explicit money_alloc_data(const money_t&); //todo
        explicit money_alloc_data(const money_t&, const priority_t&); //todo
        ~money_alloc_data();
        
        money_alloc_data& operator=(const money_alloc_data&); //todo
        bool operator==(const money_alloc_data&) const;       //todo
        bool operator!=(const money_alloc_data&) const;       //todo
        bool operator<(const money_alloc_data&) const;        //todo    
        bool operator<=(const money_alloc_data&) const;       //todo
        bool operator>(const money_alloc_data&) const;        //todo
        bool operator>=(const money_alloc_data&) const;       //todo
        
        money_t value;
        std::pair<bool, float> percent; //percent: can be turned on/off
        priority_t priority;
        
    } money_alloc_data;
    
    typedef class budget_class
    {
    public:
        explicit budget_class(); //todo
        ~budget_class();         //todo
        
        void devide_equally(); //todo
        void apply_percentages();  //todo
        void set_all_percents(const bool&);  //todo
        money_t money_left() const;  //todo
        
        money_t total; //total amount of money to spend
    private:
        std::vector<money_alloc_data> allocations;
        tdata::time_class date;
    } budget_class;
}

#endif