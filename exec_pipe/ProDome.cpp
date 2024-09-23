#include<unistd.h>
#include<iostream>
#include<vector>
#include<string>
using namespace std;
#include <sys/wait.h>
#include <sys/types.h>
#include<string.h>

#define MSG_SIZE 7
#define processnum 5
// 描述
class channel
{
public:
    channel(int cmdfd, pid_t id, std::string& process_name)
        : _cmdfd(cmdfd), _id(id), _process_name(process_name)
    {

    }

public:
    int _cmdfd;
    pid_t _id;
    std::string _process_name;
};

bool fork_Contral_slaver(int fd)
{
    while (true)
    {
        char msg[1024];
        ssize_t msg_size = read(fd, msg, sizeof(msg));

        if (msg_size <= 0) {
            perror("read error:");
            return false; // 处理读取错误
        }


        int pipefd[2] = {0};
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
            return false;
        }

        pid_t id = fork();
        if (id == 0)
        {
            close(pipefd[1]);
            char msg_child[1024];
            ssize_t msg_Size = read(pipefd[0], msg_child, sizeof(msg_child));
            msg_child[msg_Size] = '\0'; // 确保字符串结束

            char *argv[] = {msg_child, NULL}; // 使用 msg_child
            execvp(argv[0], argv);
            perror("execvp failed"); // 处理 execvp 错误
            exit(1); // 确保子进程退出
        }

        close(pipefd[0]);
        msg[msg_size] = '\0'; // 确保字符串结束
        cout << "child: " << msg << endl;
        write(pipefd[1], msg, msg_size); // 修改为 pipefd[1]

    }

    return true;
}

void InitPrcoess(std::vector<channel>& channels)
{
    for (int i = 0; i < processnum; i++) // 使用 processnum
    {
        int pipefd[2] = {0};
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
            return;
        }

        pid_t id = fork();
        if (id == 0)
        {
            close(pipefd[1]);
            fork_Contral_slaver(pipefd[0]);
            close(pipefd[0]);
            exit(0); // 确保子进程退出
        }

        close(pipefd[0]);
        std::string name = "process" + std::to_string(id) + std::to_string(i);
        channels.push_back(channel(pipefd[1], id, name));
    }
}

void Ctslaver(const vector<channel>& channels)
{
    //轮转分配
    int which = 0;
    while (true)
    {
        char msg[1024];
        cout << "替换程序进程:";
        std::cin.getline(msg, sizeof(msg));
        cout << msg << endl;

        ssize_t bytes_written = write(channels[which]._cmdfd, msg, strlen(msg));
        if (bytes_written < 0) {
            std::cerr << "写入失败" << std::endl;
        } else {
            std::cout << "task allocate -> " << msg << "  -> " << channels[which]._process_name << std::endl;
        }

        which++;

        which %= 5;
        sleep(1);
    }
}


void Quslaver(const vector<channel>& channels)
{
    for(auto&e : channels)
    {
        close(e._cmdfd);
        waitpid(e._id, nullptr, 0);
    }
}


int main()
{
    std::vector<channel> channels;
    InitPrcoess(channels);
    Ctslaver(channels);
    // char *args[] = {"./a.out",NULL};
    // execv(args[0], args);
    Quslaver(channels);

    return 0;
}