#include "commt.hpp"

using namespace std;

int main()
{
    int time = TIME_OUT;
    int fd = 0;
    while (time)
    {
        fd = open(FIFO_FILE, O_WRONLY | O_APPEND);
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
        return -1;
    }

    cout << "client open sucess:" << endl;

    while(true)
    {
        string line;
        cout << "Please Enter@ ";
        getline(cin, line);

        write(fd, line.c_str(), line.size());
    }

    close(fd);

    return 0;
}