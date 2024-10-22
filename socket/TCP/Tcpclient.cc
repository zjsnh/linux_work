#include <iostream>
using namespace std;
#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// enum 错误列表


struct Link
{
    struct sockaddr_in server;
    int sockfd;
};

void tips()
{
    cout << "\n\r ./client + IP + 端口" << endl;
}

void* Recv(void* args)
{
    Link *L = static_cast<Link *>(args);
    char inbuffer[4096];
    while(true)
    {
        memset(inbuffer, 0, sizeof(inbuffer) - 1);
        int n = read(L->sockfd, inbuffer, sizeof(inbuffer) - 1);
        if(n > 0)
        {
            inbuffer[n] = '\0';
            cerr << inbuffer << endl;
        }
        else if(n == 0)
        {
            cerr << "connect closed !" << endl;
            exit(0);
        }
    }
}

void* Send(void* args)
{
    Link *L = static_cast<Link *>(args);
    string message;
    

    while(true)
    {
        cout << "client #:";
        message.clear();
        getline(cin, message);
        int n = write(L->sockfd, (void *)message.c_str(), message.size());
        if(n < 0)
        {
            cerr << "write faile" << endl;
        }
    }
}

int main(int argc,char* argv[])
{
    if (argc != 3)
    {
        tips();
        exit(1);
    }

    Link information;
    
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    // struct sockaddr_in server;
    memset(&(information.server), 0, sizeof(information.server));
    information.server.sin_family = AF_INET;
    information.server.sin_port = htons(serverport);
    inet_pton(AF_INET, serverip.c_str(), &(information.server.sin_addr));


    int cnt = 5;
    int isreconnect = false;
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 1;
    }
    do
    {

        int n = connect(sockfd, (struct sockaddr *)&(information.server), sizeof(information.server));
        if (n < 0)
        {
            isreconnect = true;
            cnt--;
            std::cerr << "connect error..., reconnect: " << cnt << std::endl;
            sleep(2);
        }
        else
        {   information.sockfd = sockfd;
            break;
        }

    } while (cnt && isreconnect);  //重连


    pthread_t r, s;
    pthread_create(&r, NULL, Recv, &information);
    pthread_create(&s, NULL, Send, &information);

    pthread_join(r, NULL);
    pthread_join(s, NULL);

    return 0;
}