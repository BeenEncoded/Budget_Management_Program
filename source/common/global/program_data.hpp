#ifndef COMMON_GLOBAL_PROGRAM_DATA_HPP_INCLUDED
#define COMMON_GLOBAL_PROGRAM_DATA_HPP_INCLUDED
#include <string>
#include <vector>

namespace global
{
    typedef struct program_data program_data;
    
    std::vector<std::string> budget_paths(const std::string&);
    
    
    typedef struct program_data
    {
        //constructors
        program_data(const program_data&) noexcept;
        program_data(program_data&&) noexcept;
        explicit program_data() noexcept;
        
        ~program_data();
        
        //assignment
        program_data& operator=(const program_data&);
        program_data& operator=(program_data&&) noexcept;
        
        //data:
        std::string folder, budget_folder;
        std::vector<std::string> budget_files;
    } program_data;
    
    
}

#endif