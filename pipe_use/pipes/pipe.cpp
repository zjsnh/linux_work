#include<iostream>
#include<unistd.h>
using namespace std;
#include<string.h>
#include<string>

void Write(int* pipefd)
{
    char buffer[1024];
    string s("hello this msg is:");
    int number = 0;

    while(true)
    {

        buffer[0] = 0;

        snprintf(buffer, sizeof(buffer), "%s--%d--%d", s.c_str(), getpid(), number++);
        write(pipefd[1], buffer, strlen(buffer));
        //cout << buffer << endl;

        
    }
}


void Read(int* pipefd)
{
    char buffer[1024];
    while(true)
    {
        buffer[0] = 0;

        ssize_t n = read(pipefd[0], buffer, sizeof(buffer));

        
        if(n > 0)
            cout << "the receive msg from : " << buffer << endl;
        
        sleep(3);
    }
}


int main()
{
    int pipefd[2] = {0};
    int n = pipe(pipefd);
    if(n < 0)
    {
        perror("pipe creat fail:");
        return 1;
    }

    pid_t id = fork();
    if(id < 0)
    {
        perror("fork fail:");
        return 1;
    }
    else if(id == 0)
    {
        //child
        close(pipefd[0]);

        Write(pipefd);
        // Write;
        close(pipefd[1]);
    }

    close(pipefd[1]);

    Read(pipefd);
    // Read
    close(pipefd[0]);

    return 0;
}
