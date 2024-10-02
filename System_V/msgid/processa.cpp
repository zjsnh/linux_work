
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

void msgbuf_set(struct _msgbuf &msg)
{
    msg.mtype = 1;  //进程a标识发送信息为 1
}

int main()
{
    int msgid = Create_msgid();
    struct _msgbuf msg;

    msgbuf_set(msg);
    while (true)
    {
        // cout << "请输入发送人: " << endl;
        // cin >> msg.mtext.send_name;
        // cout << "请输入发送信息" << endl;
        // cin >> msg.mtext.data;
        // msgsnd(msgid, &msg, sizeof(msg), 0);

        struct _msgbuf msg;
        //msg.mtype = 1; // 设置消息类型
        strcpy(msg.mtext.send_name, "Sender");
        strcpy(msg.mtext.data, "Hello");

        msgsnd(msgid, &msg, sizeof(msg.mtext), 0);

    }

    return 0;
}