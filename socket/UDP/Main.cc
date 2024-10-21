#include <iostream>
using namespace std;


#include <memory> // 智能指针
#include "socket_server.hpp"

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

    std::unique_ptr<Udp_server> svr(new Udp_server((uint16_t)atoi(argv[1])));


    svr->Init();
    svr->Run();

    return 0;
}