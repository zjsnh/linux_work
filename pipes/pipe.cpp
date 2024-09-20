#include<iostream>
#include<unistd.h>
using namespace std;
#include<string.h>

int main()
{
    int pipefd[2];

    char buf[1024];
    pid_t id;

    //管道判断
    if(pipe(pipefd) == -1)
    {
        perror("piepe creat fail");
        return 1;
    }

    id = fork();

    if(id == -1)
    {
        perror("fork fail");
        return 1;
    }
    else if(id == 0)
    {
        close(pipefd[1]);

        read(pipefd[0], buf, sizeof(buf));
        std::cout << "子进程接收到的数据: " << buf << endl;

        close(pipefd[0]);
    }
    else{
        close(pipefd[0]);
        const char *msg = "hello child fork";
        cout << "我将要传递的信息" << msg << endl;

        write(pipefd[1], msg, strlen(msg));

        close(pipefd[1]);
    }

    return 0;
}