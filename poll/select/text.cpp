#include "SelectServer.hpp"
#include <iostream>
#include <memory>



int main(int argc,char* argv[])
{

    std::cout << argv[1] << std::endl;
    std::unique_ptr<SelectServer> ptr(new SelectServer((uint16_t)atoi(argv[1])));

    ptr->Init();
    ptr->Run();

    return 0;
}