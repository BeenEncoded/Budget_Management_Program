#if UNIT_TEST_PROG == true
#include "tests/unit_tests.hpp"
#endif

#include <utility>

#include "common/global/program_data.hpp"
#include "menus/budget_menu.hpp"


namespace
{
    global::program_data load_program_data();
    int start_prog();
    
    
    /**
     * @brief Returns a global::program_data that is initialized to program defaults.
     * This function will also be the place where the data is loaded from files, 
     * should program_data be loaded from a file in the future.
     * @return the default program data.
     */
    inline global::program_data load_program_data()
    {
        //a place for future initialization, should that be added.
        global::program_data pdat;
        pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
        return pdat;
    }
    
    __attribute__((unused)) inline int start_prog()
    {
        global::program_data pdat(std::move(load_program_data()));
        return !menu::budget_list_menu(pdat);
    }
    
}


int main(int count __attribute__((unused)), char **vec __attribute__((unused)))
{
#if UNIT_TEST_PROG == true
    run_tests();
    return 0;
#else
    return start_prog();
#endif
}

