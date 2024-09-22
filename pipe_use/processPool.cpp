#include <iostream>
using namespace std;
#include <string>
#include <unistd.h>
#include <vector>
#include <cassert>
#include"Task.hpp"
#include <ctime>
#include <sys/wait.h>
#include <sys/types.h>

#define process_num 5

//描述
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

void serve()
{
    while(true)
    {
        int task_pos;
        ssize_t n = read(0, &task_pos, sizeof(int));
        if(n==sizeof(task_pos))
        {
            task_list[task_pos]();
        }
    }
}

void Initslaver(vector<channel>& channels)
{
    for (int i = 0; i < process_num;i++)
    {
        int pipefd[2];
        int n = pipe(pipefd);
        assert(!n);
        pid_t id = fork();
        if(id == 0)
        {
            close(pipefd[1]);
            dup2(pipefd[0], 0);
            serve();

            close(pipefd[0]);
        }


        close(pipefd[0]);

        std::string process_name("process-" + to_string(id) + "-" + to_string(i));
        channels.push_back(channel(pipefd[1], id, process_name));
    }
}

void Ctslaver(const vector<channel>& channels)
{
    //轮转分配
    int which = 0;
    while (true)
    {
        int task_pos = rand() % 4 + 1;
        write(channels[which]._cmdfd, &task_pos, sizeof(int));
        cout << "task allocate ->" << task_pos << "  -> " << channels[which]._process_name << endl;
        which++;

        which %= 5;
        sleep(1);
    }
}

void debug(const vector<channel>& channels){
    for (auto &e : channels)
    {
        cout << e._cmdfd << " " << e._id << " " << e._process_name << endl;
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
    srand(time(nullptr) ^ getpid());
    vector<channel> channels;
    Initslaver(channels);

    //debug(channels);
    Ctslaver(channels);

    Quslaver(channels);

    return 0;
}
