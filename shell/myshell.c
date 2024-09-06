#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


#define LEFT "["
#define RIGHT "]"
#define PWD_CUT "/"
#define CLINE_MAX 1024
#define ARGV_MAX 32
#define CLINE_CUT "     "
#define EXIT_CODE 44 // 


char commandline[CLINE_MAX];
char* argv[ARGV_MAX];
int lastcode = 0;


char* getUsername()
{
    return getenv("USER");
}

char* getPwd()
{
    char* pwd_env = getenv("PWD");
    if (!pwd_env) return NULL;  // 检查是否成功获取到环境变量

    char* pwd_copy = strdup(pwd_env);  // 复制环境变量的值，确保不会修改原始数据
    if (!pwd_copy) return NULL;        // 确保内存分配成功

    char* token = strtok(pwd_copy, PWD_CUT);  // 使用 PWD_CUT 作为分隔符进行分割
    char* last_token = token;

    while (token != NULL) 
    {
        last_token = token;             // 保存当前的分割部分
        token = strtok(NULL, PWD_CUT);  // 获取下一个分割部分
    }

    return last_token;  // 返回最后一个分割部分
}

char* getHostName()
{
    return getenv("HOSTNAME");
}

void interact(int size)
{
    printf(LEFT"%s@%s %s"RIGHT"""""# ",getUsername(),getHostName(),getPwd());
    char* s = fgets(commandline,size,stdin);
    (void)s;

assert(s);
    commandline[strlen(commandline)-1] = '\0';

    //printf("%s",commandline);
}

int splitstring(char cline[], char *_argv[])
{
    int i = 0;
    argv[i++] = strtok(cline, CLINE_CUT);
    while(_argv[i++] = strtok(NULL, CLINE_CUT)); // 故意写的=
    return i - 1;
}

void NormalExcute(char *_argv[])
{
    pid_t id = fork();
    if(id < 0){
        perror("fork");
        return;
    }
    else if(id == 0){
        //让子进程执行命令
        //execvpe(_argv[0], _argv, environ);
        execvp(_argv[0], _argv);
        exit(EXIT_CODE);
    }
    else{
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        if(rid == id) 
        {
            lastcode = WEXITSTATUS(status);
        }
    }
}

int main()
{
    while(1)
    {
        interact(sizeof(commandline));
        int argc = splitstring(commandline,argv);
        if(argc == 0)
            continue;





        NormalExcute(argv);
    }
    return 0;
}
