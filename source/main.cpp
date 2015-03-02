#include "build_defines.hpp"

#if UNIT_TEST_PROG == true
#include "tests/unit_tests.hpp"
#endif

int main(int count __attribute__((unused)), char **vec __attribute__((unused)))
{
#if UNIT_TEST_PROG == true
    run_tests();
#endif
    return 0;
}