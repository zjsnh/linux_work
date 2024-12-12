//#include "Any.hpp"
#include <iostream>

//C++ 17
#include <any>


class text
{
public:
    text() { std::cout << "构造" << std::endl; }
    text(const text &_text) { std::cout << "拷贝构造" << std::endl; }
    //text(text&& _text) { std::cout << "移动构造" << std::endl; }

    ~text() { std::cout << "析构" << std::endl; }
};

int main()
{
    // std::any a;
    // a = 10;

    // int *p = std::any_cast<int>(&a);
    // std::cout << *p << std::endl;

    std::cout << sizeof(char *) << std::endl;
    std::cout << sizeof(int *) << std::endl;
    
    return 0;
}