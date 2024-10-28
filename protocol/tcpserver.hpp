#include "socket.hpp"


class Server
{
public:
    Server(uint16_t port = _port,std::string ip = _ip)
        :socket_(_port,_ip)
    {}

    void Init()
    {
        socket_.Bind();
        socket_.Listen();
    }

    //多进程
    void Run()
    {
        while(true)
        {
            int sockfd = socket_.Accept();
        }
    }


private:
    Socket socket_;
};