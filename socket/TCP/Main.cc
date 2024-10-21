#include <iostream>
using namespace std;



#include <memory> // 智能指针
#include "Tcpserver.hpp"

void tips()
{
    cout << "\n\r ./server + 端口" << endl;
}

int main(int argc,char* argv[])
{
    if (argc != 2)
    {
        tips();
        exit(1);
    }

    std::unique_ptr<TCP_server> svr(new TCP_server((uint16_t)atoi(argv[1])));


    svr->Init();
    svr->Run();

    return 0;
}