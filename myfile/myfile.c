#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include<string.h>
#include<unistd.h>


int main()
{

    //printf("stdin : %d\n",stdin->_fileno);
    //printf("stdout : %d\n",stdout->_fileno);
    //printf("stderr : %d\n",stderr->_fileno);
    /*umask(0);
    int fd = open("normal.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
    int fd2 = open("error.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
    const char *message = "normal message\n";
    write(1,message,strlen(message));
    write(1,message,strlen(message));
    write(1,message,strlen(message));
    write(1,message,strlen(message));
    write(1,message,strlen(message));
    const char *error_msg = "error message\n";
    write(2,error_msg,strlen(error_msg));
    write(2,error_msg,strlen(error_msg));
    write(2,error_msg,strlen(error_msg));
    write(2,error_msg,strlen(error_msg));
    write(2,error_msg,strlen(error_msg));
    close(fd);
    close(fd2);*/

    char msg[1024];
    fscanf(stdin,"%s",msg);


    printf("%s\n",msg);

    return 0;
}
