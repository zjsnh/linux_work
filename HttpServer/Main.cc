#include "httpServer.hpp"


// port 

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cout << "\r\n"
                  << argv[0] << "+ port" << std::endl;
    }

    u_int16_t port = (u_int16_t)atoi(argv[1]);
    HttpServer *hvr = new HttpServer(port);
    hvr->Init();
    hvr->Start();

    return 0;
}