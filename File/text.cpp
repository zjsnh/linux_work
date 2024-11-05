#include <iostream>
#include "util.hpp"


int main()
{
    std::string filename = "../File/makefile";
    Cloud::FileUtil fu(filename);

    std::cout << fu.FileSize() << std::endl;
    std::cout << fu.LastMTime() << std::endl;
    std::cout << fu.LastATime() << std::endl;
    std::cout << fu.FormatTime(fu.LastMTime()) << std::endl;
    std::cout << fu.FormatTime(fu.LastATime()) << std::endl;

    std::cout << fu.FileName() << std::endl;

    return 0;
}