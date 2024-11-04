#include"tcpserver.hpp"
#include <unistd.h>
#include <iostream>
#include "calculator.hpp"
// #include "Daemon.hpp"




static void Usage(const std::string &proc)
{
    std::cout << "\nUsage: " << proc << " port\n" << std::endl; 
}

// ./servercal 8080
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);
    calculator cal;

    Server *tcps = new Server(port, std::bind(&calculator::Calculator, &cal, std::placeholders::_1));

    tcps->Init();
    tcps->Run();

    return 0;
}