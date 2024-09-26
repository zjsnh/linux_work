#include "commt.hpp"
//#include "log.hpp"

using namespace std;


// void channel_definition(log &_log,string& channel)
// {
//     cout << "请输入"<< channel <<"信道"<< endl;
//     string _channel;
//     cin >> _channel;

//     channel += _channel;
//     int cin_time = CIN_TIMES;
//     if (!send_channel.empty() && cin_time)
//     {
//         _log(Info, "The user send channel is not entered!!!"); 
//         cout << "请重新输入输入你的发送管道" << endl;
//         cin >> send_channel;
//         cin_time--;

//         if(cin_time == 0)
//         {
//             cout << "输入错误次数过多!" << endl;
//             return 0;
//         }
//     }
// }


//可以判断文件里是否存在 定义好的通信信道，然后在创建子进程去创建信道

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

            fd = open(SEND, O_WRONLY | O_APPEND);
            if(fd < 0)
            {
                cout << "tying link server..." << endl;
            }
            else{
                break;
            }

            sleep(1);
            time--;
        }

        if(fd < 0)
        {
            perror("client open fail");
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
        int fd = open(REVERSE, O_RDONLY);
        if(fd < 0)
        {
            _log(Fatal, "file open fail:");
            exit(-1);
        }
        cout << "Send channel open success" << endl;
        //通信
        

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

    Init init;
    // string send_channel("send");
    // string rever_channel("reverse");

    //int fd_fifo[2] = {0};
    //int n = pipe(fd_fifo); // ->  子进程返回 发送命名管道和接收命名管道

    //fork() 1： 子进程 发送信息
    //fork() 2:  接收信息
    send();
    reverse();

    //回收

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