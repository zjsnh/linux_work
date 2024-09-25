#include "commt.hpp"
#include "log.hpp"

using namespace std;
int main()
{
    Init init;

    log _log;

    int fd = open(FIFO_FILE, O_RDONLY);
    if(fd < 0)
    {
        perror("file open fail:");
        return -1;
    }
    cout << "mkfifo_file open success" << endl;
    //通信

    _log(0, "好人一生平安%s : %d",strerror(errno),errno);
    _log(1, "好人一生平安%s : %d",strerror(errno),errno);
    _log(2, "好人一生平安%s : %d",strerror(errno),errno);
    _log(3, "好人一生平安%s : %d",strerror(errno),errno);
    _log(4, "好人一生平安%s : %d",strerror(errno),errno);
    

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

    
    return 0;
}