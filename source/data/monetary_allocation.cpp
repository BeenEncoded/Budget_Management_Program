#include <string>
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <utility>

#include "common/common.hpp"
#include "monetary_allocation.hpp"
#include "common/global/global_defines.hpp"
#include "utility/time_class.hpp"

namespace
{
}

//money_alloc_data:
namespace data
{
    money_alloc_data::money_alloc_data(const money_alloc_data& m) noexcept : 
            name(m.name),
            value(m.value),
            id(m.id)
    {
    }
    
    money_alloc_data::money_alloc_data(money_alloc_data&& m) noexcept : 
            name(std::move(m.name)),
            value(std::move(m.value)),
            id(std::move(m.id))
    {
    }
    
    money_alloc_data::money_alloc_data(const std::string& n, const money_t& m) noexcept :
            name(n),
            value(m),
            id(0)
    {
    }
    
    money_alloc_data::money_alloc_data() noexcept : 
            name(),
            value(0),
            id(0)
    {
    }
    
    money_alloc_data::~money_alloc_data()
    {
    }
    
    money_alloc_data& money_alloc_data::operator=(const money_alloc_data& m)
    {
        if(this != &m)
        {
            this->value = m.value;
            this->name = m.name;
            this->id = m.id;
        }
        return *this;
    }
    
    money_alloc_data& money_alloc_data::operator=(money_alloc_data&& m) noexcept
    {
        this->value = std::move(m.value);
        this->name = std::move(m.name);
        this->id = std::move(m.id);
        return *this;
    }
    
    bool money_alloc_data::operator==(const money_alloc_data& m) const noexcept
    {
        return ((this->name == m.name) && 
                (this->value == m.value) && 
                (this->id == m.id));
    }
    
    bool money_alloc_data::operator!=(const money_alloc_data& m) const noexcept
    {
        return ((this->name != m.name) || 
                (this->value != m.value) || 
                (this->id != m.id));
    }
    
    std::ostream& operator<<(std::ostream& out, const money_alloc_data& m)
    {
        using common::out_mem;
        
        if(out.good())
        {
            out_mem<money_alloc_data::ID_T>(out, m.id);
            out_mem<money_t>(out, m.value);
            out<< m.name<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, money_alloc_data& m)
    {
        using common::safe_getline;
        using common::in_mem;
        
        m = money_alloc_data();
        if(in.good())
        {
            in_mem<money_alloc_data::ID_T>(in, m.id);
            in_mem<money_t>(in, m.value);
            safe_getline(in, m.name, mem_delim::value);
        }
        return in;
    }
    
    
}

//budget data
namespace data
{
    budget_data::budget_data(const budget_data& b) noexcept :
            total_money(b.total_money),
            allocs(b.allocs),
            timestamp(b.timestamp)
    {
    }
    
    budget_data::budget_data(budget_data&& b) noexcept : 
            total_money(std::move(b.total_money)),
            allocs(std::move(b.allocs)),
            timestamp(std::move(b.timestamp))
    {
    }
    
    budget_data::budget_data(const money_t& m) noexcept : 
            total_money(m),
            allocs(),
            timestamp(tdata::current_time())
    {
    }
    
    budget_data::budget_data() noexcept :
            total_money(0),
            allocs(),
            timestamp(tdata::current_time())
    {
    }
    
    budget_data::~budget_data()
    {
    }
    
    budget_data& budget_data::operator=(const budget_data& b)
    {
        if(this != &b)
        {
            this->total_money = b.total_money;
            this->allocs = b.allocs;
            this->timestamp = b.timestamp;
        }
        return *this;
    }
    
    budget_data& budget_data::operator=(budget_data&& b) noexcept
    {
        this->allocs = std::move(b.allocs);
        this->timestamp = std::move(b.timestamp);
        this->total_money = std::move(b.total_money);
        return *this;
    }
    
    bool budget_data::operator==(const budget_data& b) const noexcept
    {
        return ((this->total_money == b.total_money) && 
                (this->allocs == b.allocs) && 
                (this->timestamp == b.timestamp));
    }
    
    bool budget_data::operator!=(const budget_data& b) const noexcept
    {
        return ((this->allocs != b.allocs) || 
                (this->total_money != b.total_money) || 
                (this->timestamp != b.timestamp));
    }
    
    std::ostream& operator<<(std::ostream& out, const budget_data& b)
    {
        using common::out_mem;
        
        if(out.good())
        {
            out_mem<money_t>(out, b.total_money);
            out<< b.timestamp;
            for(std::vector<money_alloc_data>::const_iterator it(b.allocs.begin()); ((it != b.allocs.end()) && out.good()); ++it)
            {
                out<< (*it);
            }
            out<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, budget_data& b)
    {
        using common::in_mem;
        
        b = budget_data();
        in.peek();
        if(in.good())
        {
            in_mem<money_t>(in, b.total_money);
            if(in.good()) in>> b.timestamp;
            
            in.peek();
            while(in.good() && (in.peek() != EOF) && (in.peek() != mem_delim::value))
            {
                b.allocs.emplace_back();
                in>> b.allocs.back();
                in.peek();
            }
            if(in.peek() == mem_delim::value)
            {
                in.get();
                in.peek();
            }
        }
        return in;
    }
    
    
}

namespace data
{
    money_alloc_data::ID_T new_alloc_id(const std::vector<money_alloc_data>& allocs)
    {
        using data::money_alloc_data;
        
        if(allocs.empty()) return 1;
        
        money_alloc_data::ID_T id(1);
        std::unordered_set<money_alloc_data::ID_T> ids;
        
        for(std::vector<money_alloc_data>::const_iterator it(allocs.begin()); it != allocs.end(); ++it)
        {
            ids.emplace(it->id);
        }
        while(ids.find(id) != ids.end()) ++id;
        return id;
    }
    
    
}

