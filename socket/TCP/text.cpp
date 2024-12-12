#include <iostream>
#include "log.hpp"



int main()
{
    std::string s("birthday");
    _log(Info, "log text,%d: %s", 20041221, s.c_str());

    return 0;
}