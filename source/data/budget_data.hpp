#ifndef MONETARY_ALLOCATION_HPP_INCLUDED
#define MONETARY_ALLOCATION_HPP_INCLUDED
#include <vector>
#include <string>
#include <iostream>

#include "utility/time_class.hpp"

namespace data
{
    typedef long long money_t;
    
    typedef struct money_alloc_data money_alloc_data;
    typedef struct budget_data budget_data;
    typedef struct alloc_statistics_data alloc_statistics_data;
    typedef struct distribution_data distribution_data;
    
    std::istream& operator>>(std::istream&, money_alloc_data&);
    std::ostream& operator<<(std::ostream&, const money_alloc_data&);
    
    std::istream& operator>>(std::istream&, budget_data&);
    std::ostream& operator<<(std::ostream&, const budget_data&);
    
    std::istream& operator>>(std::istream&, std::vector<money_alloc_data>&);
    std::ostream& operator<<(std::ostream&, const std::vector<money_alloc_data>&);
    
    std::istream& operator>>(std::istream&, std::vector<budget_data>&);
    std::ostream& operator<<(std::ostream&, const std::vector<budget_data>&);
    
    void generate_meta_data(budget_data&);
    void delete_meta_data(budget_data&);
    
    
    /**
     * @class distrubution_data
     * @author Jonathan Whitlock
     * @date 06/10/2015
     * @file budget_data.hpp
     * @brief Used for the storage of data regarding budget money distribution
     * for each allocation.
     */
    typedef struct distribution_data
    {
        typedef short percent_t;
        
        enum distribution_type
        {
            none = 0,
            bpercent,
            equally
        };
        
        explicit distribution_data();
        distribution_data(const distribution_data&);
        distribution_data(distribution_data&&) noexcept;
        
        ~distribution_data();
        
        distribution_data& operator=(const distribution_data&);
        distribution_data& operator=(distribution_data&&) noexcept;
        
        std::vector<std::string> distribution_name() const;
        
        bool enabled;
        distribution_type dist_t; //todo remove this
        percent_t percent_value;
    } distribution_data;
    
    /**
     * @class alloc_statistics_data
     * @author Jonathan Whitlock
     * @date 06/09/2015
     * @file budget_data.hpp
     * @brief Meant to store statistics on money_alloc_data that is calculated
     * on a per-allocation basis.  These values are volatile, so they aren't
     * saved as part of the allocation data.  The data stored here should
     * never be independent of the budget that a money_alloc_data belongs to.
     */
    typedef struct alloc_statistics_data
    {
        explicit alloc_statistics_data();
        alloc_statistics_data(const alloc_statistics_data&);
        alloc_statistics_data(alloc_statistics_data&&) noexcept;
        
        ~alloc_statistics_data();
        
        alloc_statistics_data& operator=(const alloc_statistics_data&);
        alloc_statistics_data& operator=(alloc_statistics_data&&) noexcept;
        
        money_t balance;
        distribution_data dist_data;
    } alloc_statistics_data;
    
    /**
     * @class money_alloc_data
     * @author Jonathan Whitlock
     * @date 03/28/2015
     * @file monetary_allocation.hpp
     * @brief Stores a single allocation of money.
     */
    typedef struct money_alloc_data
    {
        typedef unsigned long long ID_T;
        
        money_alloc_data(const money_alloc_data&) noexcept;
        money_alloc_data(money_alloc_data&&) noexcept;
        explicit money_alloc_data(const std::string&, const money_t&) noexcept;
        explicit money_alloc_data() noexcept;
        
        ~money_alloc_data();
        
        money_alloc_data& operator=(const money_alloc_data&);
        money_alloc_data& operator=(money_alloc_data&&) noexcept;
        bool operator==(const money_alloc_data&) const noexcept;
        bool operator!=(const money_alloc_data&) const noexcept;
        
        std::string name;
        money_t value;
        ID_T id;
        alloc_statistics_data *meta_data;
    } money_alloc_data;
    
    /**
     * @class budget_data
     * @author Jonathan Whitlock
     * @date 03/28/2015
     * @file monetary_allocation.hpp
     * @brief Stores data regarding a budget.
     */
    typedef struct budget_data
    {
        typedef unsigned long long ID_T;
        
        budget_data(const budget_data&) noexcept;
        budget_data(budget_data&&) noexcept;
        explicit budget_data() noexcept;
        explicit budget_data(const money_t&) noexcept;
        
        ~budget_data();
        
        budget_data& operator=(const budget_data&);
        budget_data& operator=(budget_data&&) noexcept;
        bool operator==(const budget_data&) const noexcept;
        bool operator!=(const budget_data&) const noexcept;
        
        money_t total_money;
        std::vector<money_alloc_data> allocs;
        tdata::time_class timestamp;
        ID_T id;
        //todo add length of time a budget spans
    } budget_data;
    
    money_alloc_data::ID_T new_alloc_id(const std::vector<money_alloc_data>&);
    budget_data::ID_T new_budget_id(const std::vector<budget_data>&);
    
    
}

#endif