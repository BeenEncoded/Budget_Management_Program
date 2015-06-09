#include <string>
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <utility>

//#include "common/common.hpp"
#include "utility/stream_operations.hpp"
#include "budget_data.hpp"
#include "common/global/global_defines.hpp"
#include "utility/time_class.hpp"
#include "utility/filesystem.hpp"

namespace
{
}

//alloc_statistics_data
namespace data
{
    alloc_statistics_data::alloc_statistics_data() : 
            balance{0}
    {
    }
    
    alloc_statistics_data::alloc_statistics_data(const alloc_statistics_data& a) : 
            balance{a.balance}
    {
    }
    
    alloc_statistics_data::alloc_statistics_data(alloc_statistics_data&& a) noexcept :
            balance{std::move(a.balance)}
    {
    }
    
    alloc_statistics_data::~alloc_statistics_data()
    {
    }
    
    alloc_statistics_data& alloc_statistics_data::operator=(const alloc_statistics_data& a)
    {
        if(this != &a)
        {
            this->balance = a.balance;
        }
        return *this;
    }
    
    alloc_statistics_data& alloc_statistics_data::operator=(alloc_statistics_data&& a) noexcept
    {
        if(this != &a)
        {
            this->balance = std::move(a.balance);
        }
        return *this;
    }
    
    
}

//money_alloc_data:
namespace data
{
    money_alloc_data::money_alloc_data(const money_alloc_data& m) noexcept : 
            name{m.name},
            value{m.value},
            id{m.id},
            meta_data{((m.meta_data == nullptr) ? nullptr : (new alloc_statistics_data(*(m.meta_data))))}
    {
    }
    
    money_alloc_data::money_alloc_data(money_alloc_data&& m) noexcept : 
            name{std::move(m.name)}, 
            value{std::move(m.value)}, 
            id{std::move(m.id)}, 
            meta_data{((m.meta_data == nullptr) ? nullptr : (new alloc_statistics_data(*(m.meta_data))))}
    {
    }
    
    money_alloc_data::money_alloc_data(const std::string& n, const money_t& m) noexcept :
            name{n}, 
            value{m}, 
            id{0}, 
            meta_data{nullptr}
    {
    }
    
    money_alloc_data::money_alloc_data() noexcept : 
            name{},
            value{0},
            id{0}, 
            meta_data{nullptr}
    {
    }
    
    money_alloc_data::~money_alloc_data()
    {
        if(this->meta_data != nullptr)
        {
            delete this->meta_data;
            this->meta_data = nullptr;
        }
    }
    
    money_alloc_data& money_alloc_data::operator=(const money_alloc_data& m)
    {
        if(this != &m)
        {
            this->value = m.value;
            this->name = m.name;
            this->id = m.id;
            if(m.meta_data != nullptr)
            {
                if(this->meta_data == nullptr) this->meta_data = new alloc_statistics_data;
                (*(this->meta_data)) = (*(m.meta_data));
            }
            else
            {
                if(this->meta_data != nullptr)
                {
                    delete this->meta_data;
                    this->meta_data = nullptr;
                }
            }
        }
        return *this;
    }
    
    money_alloc_data& money_alloc_data::operator=(money_alloc_data&& m) noexcept
    {
        this->value = std::move(m.value);
        this->name = std::move(m.name);
        this->id = std::move(m.id);
        
        /* Because this object is responsible for deleting the pointer, we can't
         * move the pointed-to object. */
        if(m.meta_data != nullptr)
        {
            if(this->meta_data == nullptr) this->meta_data = new alloc_statistics_data;
            (*(this->meta_data)) = (*(m.meta_data));
        }
        else
        {
            if(this->meta_data != nullptr)
            {
                delete this->meta_data;
                this->meta_data = nullptr;
            }
        }
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
        using utility::out_mem;
        
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
        using utility::safe_getline;
        using utility::in_mem;
        
        m = money_alloc_data();
        if(in.good())
        {
            in_mem<money_alloc_data::ID_T>(in, m.id);
            if(in.good()) in_mem<money_t>(in, m.value);
            if(in.good())
            {
                if(!safe_getline(in, m.name, mem_delim::value))
                {
                    if(!in.fail()) 
                    {
                        in.setstate(std::ios_base::failbit);
                    }
                }
            }
        }
        return in;
    }
    
    std::ostream& operator<<(std::ostream& out, const std::vector<money_alloc_data>& v)
    {
        return utility::write_vector(out, v);
    }
    
    std::istream& operator>>(std::istream& in, std::vector<money_alloc_data>& v)
    {
        return utility::read_vector(in, v);
    }
    
    
}

//budget data
namespace data
{
    budget_data::budget_data(const budget_data& b) noexcept :
            total_money(b.total_money),
            allocs(b.allocs),
            timestamp(b.timestamp),
            id(b.id)
    {
    }
    
    budget_data::budget_data(budget_data&& b) noexcept : 
            total_money(std::move(b.total_money)),
            allocs(std::move(b.allocs)),
            timestamp(std::move(b.timestamp)),
            id(std::move(b.id))
    {
    }
    
    budget_data::budget_data(const money_t& m) noexcept : 
            total_money(m),
            allocs(),
            timestamp(tdata::current_time()),
            id(0)
    {
    }
    
    budget_data::budget_data() noexcept :
            total_money(0),
            allocs(),
            timestamp(tdata::current_time()),
            id(0)
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
            this->id = b.id;
        }
        return *this;
    }
    
    budget_data& budget_data::operator=(budget_data&& b) noexcept
    {
        this->allocs = std::move(b.allocs);
        this->timestamp = std::move(b.timestamp);
        this->total_money = std::move(b.total_money);
        this->id = std::move(b.id);
        return *this;
    }
    
    bool budget_data::operator==(const budget_data& b) const noexcept
    {
        return ((this->total_money == b.total_money) && 
                (this->allocs == b.allocs) && 
                (this->timestamp == b.timestamp) && 
                (this->id == b.id));
    }
    
    bool budget_data::operator!=(const budget_data& b) const noexcept
    {
        return ((this->allocs != b.allocs) || 
                (this->total_money != b.total_money) || 
                (this->timestamp != b.timestamp) ||
                (this->id != b.id));
    }
    
    std::ostream& operator<<(std::ostream& out, const budget_data& b)
    {
        using utility::out_mem;
        
        if(out.good())
        {
            out_mem<money_t>(out, b.total_money);
            out_mem<budget_data::ID_T>(out, b.id);
            if(out.good()) out<< b.timestamp;
            if(out.good()) out<< b.allocs;
        }
        return out;
    }
    
    /**
     * @brief reads budget_data from a stream.
     * @param in the stream to read from.
     * @param b the object to store it in.
     * @return a stream.  Failbit is set if the read fails.
     */
    std::istream& operator>>(std::istream& in, budget_data& b)
    {
        using std::ios_base;
        using utility::in_mem;
        
        b = budget_data();
        in.peek();
        if(in.good())
        {
            if(in.good()) in_mem<money_t>(in, b.total_money);
            if(in.good()) in_mem<budget_data::ID_T>(in, b.id);
            if(in.good()) in>> b.timestamp;
            if(in.good()) in>> b.allocs;
        }
        return in;
    }
    
    std::ostream& operator<<(std::ostream& out, const std::vector<budget_data>& v)
    {
        return utility::write_vector(out, v);
    }
    
    std::istream& operator>>(std::istream& in, std::vector<budget_data>& v)
    {
        return utility::read_vector(in, v);
    }
    
    
}

namespace data
{
    money_alloc_data::ID_T new_alloc_id(const std::vector<money_alloc_data>& allocs)
    {
        using data::money_alloc_data;
        
        if(allocs.empty()) return 1;
        
        money_alloc_data::ID_T id(0);
        std::unordered_set<money_alloc_data::ID_T> ids;
        
        for(std::vector<money_alloc_data>::const_iterator it(allocs.begin()); it != allocs.end(); ++it) ids.insert(it->id);
        while(ids.find(++id) != ids.end());
        return id;
    }
    
    budget_data::ID_T new_budget_id(const std::vector<budget_data>& budgets)
    {
        if(budgets.empty()) return 1;
        
        budget_data::ID_T new_id{0};
        std::unordered_set<budget_data::ID_T> ids;
        
        for(std::vector<budget_data>::const_iterator it{budgets.begin()}; it != budgets.end(); ++it) ids.insert(it->id);
        while(ids.find(++new_id) != ids.end());
        return new_id;
    }
    
    
}

