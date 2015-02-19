#include <iostream>

#include "common/common.hpp"

int main(int count __attribute__((unused)), char **vec __attribute__((unused)))
{
    using std::cout;
    using std::endl;
    using std::cin;
    
    std::string temps;
    if(common::safe_getline(cin, temps, '\n')) cout<< temps<< endl;
    return 0;
}