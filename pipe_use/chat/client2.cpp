#include "commt.hpp"

using namespace std;

void send()
{
    pid_t s = fork();
    if(s < 0)
    {
        _log(Fatal, "send_fork create fail");
        exit(-1);
    }
    else if(s == 0)
    {
        int time = TIME_OUT;
        //子进程链接 send_channel
        int fd = 0;
        while (time)
        {

            fd = open(REVERSE, O_WRONLY | O_APPEND);
            if(fd < 0)
            {
                cout << "tying link send..." << endl;
            }
            else{
                break;
            }

            sleep(1);
            time--;
        }

        if(fd < 0)
        {
            _log(Fatal, "client open fail");
            exit(-1);
        }

        cout << "client open sucess:" << endl;


        while(true)
        {
            string line;
            //cout << "Please Enter@ ";
            getline(cin, line);

            write(fd, line.c_str(), line.size());
        }

        close(fd);
        return;
    }
}

void reverse()
{

    pid_t s = fork();
    if(s < 0)
    {
        _log(Fatal, "reverse_fork create fail");
        exit(-1);
    }
    else if(s == 0)
    {

        int time = TIME_OUT;

        int fd = 0;

        while (time)
        {

            fd = open(SEND, O_RDONLY);
            if(fd < 0)
            {
                cout << "tying link reverse..." << endl;
            }
            else{
                break;
            }

            sleep(1);
            time--;
            
            //通信
        
        }

        if(fd < 0)
        {
            _log(Fatal, "client open fail");
            exit(-1);
        }

        cout << "Send channel open success" << endl;
        while(true)
        {
            char msg[1024];
            int size  = read(fd, msg, sizeof(msg));
            msg[size] = '\0';
            if(size == 0)
            {
                cout << "link disconnection" << endl;
                break;
            }
            cout << "msg from client: " << msg << endl;
        }

        close(fd);
        //关闭管道文件 -> unlink
        return;
    }
}


int main()
{
    send();
    reverse();

    int fork_num = 0;
    while (true)
    {
        if(waitpid(0, NULL, WNOHANG))
        {
            fork_num++;
        }

        if(fork_num==2)
            break;
    }

    return 0;

}