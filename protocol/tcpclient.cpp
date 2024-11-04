#include <iostream>
#include "protocol.hpp"
#include "socket.hpp"
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

const std::string opers = "+-*/%=-=&^";

static void Usage(const std::string &proc)
{
    std::cout << "\nUsage: " << proc << " port\n" << std::endl; 
}


int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    srand(time(nullptr));

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    Socket sockfd;
    sockfd._socket();
    bool r = sockfd.Connect(serverip, serverport);
    if(!r) return 1;

    std::string send_buffer;
    char read_buffer[1024];
    Response res;
    while (true)
    {
        int x = rand() % 100 + 1;
        usleep(1229);
        int y = rand() % 100;
        usleep(1229);
        char op = opers[rand() % opers.size()];

        Request req(x, y, op);

        req.Serialize(send_buffer);
        Decode(send_buffer);

        cout << "+++++++++++++++++++++++++++++++++++++++" << endl;
        cout << send_buffer << std::endl;

        write(sockfd.Fd(), send_buffer.c_str(), send_buffer.size());

        int n = read(sockfd.Fd(), read_buffer, sizeof(read_buffer));
        if (n < 0)
        {
            continue;
        }

        read_buffer[n] = 0;
        std::string in = read_buffer;
        std::string buffer;
        if(!Encode(in, buffer))
        {
            continue;
        }

        std::cout << buffer << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++" << endl;

        //res.Deserialize(buffer);
        sleep(1);
    }

    return 0;
}