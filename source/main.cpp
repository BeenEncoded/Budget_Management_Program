#include "build_defines.hpp"
#if UNIT_TEST_PROG == true
#include "tests/unit_tests.hpp"
#endif

#include <utility>

#include "common/global/program_data.hpp"
#include "menus/main_menu.hpp"


namespace
{
    global::program_data load_program_data();
    int start_prog();
    
    
    inline global::program_data load_program_data()
    {
        //a place for future initialization, should that be added.
        global::program_data pdat;
        pdat.budget_files = std::move(global::budget_paths(pdat.budget_folder));
        return pdat;
    }
    
    inline int start_prog()
    {
        global::program_data pdat(std::move(load_program_data()));
        return !menu::main_menu(pdat);
    }
    
}


int main(int count __attribute__((unused)), char **vec __attribute__((unused)))
{
#if UNIT_TEST_PROG == true
    run_tests();
#endif
    return start_prog();
}

