
#include "commit.hpp"


struct msg_data
{
    char send_name[50]; // 使用字符数组
    char data[100];     // 使用字符数组
};

struct _msgbuf
{
    long mtype;
    struct msg_data mtext;
};

int main()
{
    int msgid = Get_msgid();
    struct _msgbuf msg;

    while(true)
    {
        //msgsnd(msgid, &msg, sizeof(msg), 0);

        size_t size = msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
        if(size < 0)
        {
            _log(Fatal, "msgrcv reverse msg %d : %s", errno, strerror(errno));
            exit(errno);
        }

        cout << msg.mtext.send_name << " : " << msg.mtext.data << endl;
    }

    return 0;
}