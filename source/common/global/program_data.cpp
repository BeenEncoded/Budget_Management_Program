#include <string>
#include <utility>
#include <vector>

#include "program_data.hpp"
#include "utility/filesystem.hpp"

namespace
{
    void files_of_ext(std::vector<std::string>&, const std::string&, const std::string&);
    std::string file_ext(const std::string&);
    
    
    inline void files_of_ext(std::vector<std::string>& paths, const std::string& folder, const std::string& ext)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::is_file;
        using fsys::tree_iterator_class;
        
        paths.erase(paths.begin(), paths.end());
        if(!is_symlink(folder).value && is_folder(folder).value)
        {
            for(tree_iterator_class it(folder); !it.at_end(); ++it)
            {
                if(!is_symlink(it.value()).value && is_file(it.value()).value)
                {
                    if(file_ext(it.value()) == ext) paths.push_back(it.value());
                }
            }
        }
    }
    
    inline std::string file_ext(const std::string& path)
    {
        std::string temps(path);
        std::string::size_type pos(temps.rfind('.'));
        
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos));
        }
        else temps.erase();
        return temps;
    }
    
    
}

namespace global
{
    program_data::program_data(const program_data& p) noexcept : 
            folder(p.folder),
            budget_folder(p.budget_folder),
            budget_files(p.budget_files)
    {
    }
    
    program_data::program_data() noexcept : 
            folder(fsys::current_path()),
            budget_folder(folder + fsys::pref_slash() + std::string("budgets")),
            budget_files(std::move(budget_paths(budget_folder)))
    {
    }
    
    program_data::program_data(program_data&& p) noexcept : 
            folder(std::move(p.folder)),
            budget_folder(std::move(p.budget_folder)),
            budget_files(std::move(p.budget_files))
    {
    }
    
    program_data::~program_data()
    {
    }
    
    program_data& program_data::operator=(const program_data& p)
    {
        if(this != &p)
        {
            this->folder = p.folder;
            this->budget_folder = p.budget_folder;
            this->budget_files = p.budget_files;
        }
        return *this;
    }
    
    program_data& program_data::operator=(program_data&& p) noexcept
    {
        this->folder = std::move(p.folder);
        this->budget_folder = std::move(p.budget_folder);
        this->budget_files = std::move(p.budget_files);
        return *this;
    }
    
    bool program_data::operator==(const program_data& p) const
    {
        return ((this->folder == p.folder) && 
                (this->budget_folder == p.budget_folder) && 
                (this->budget_files == p.budget_files));
    }
    
    bool program_data::operator!=(const program_data& p) const
    {
        return ((this->folder != p.folder) || 
                (this->budget_folder != p.budget_folder) || 
                (this->budget_files != p.budget_files));
    }
    
    
}

namespace global
{
    std::vector<std::string> budget_paths(const std::string& folder)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::is_file;
        using fsys::tree_iterator_class;
        
        std::vector<std::string> paths;
        
        if(!is_symlink(folder).value && is_folder(folder).value)
        {
            files_of_ext(paths, folder, ".bud");
        }
        return paths;
    }
    
    
}

