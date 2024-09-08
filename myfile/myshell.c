#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

#define LEFT "["
#define RIGHT "]"
#define PWD_CUT "/"
#define CLINE_MAX 1024
#define ARGV_MAX 32
#define CLINE_CUT "     "
#define EXIT_CODE 44 


char commandline[CLINE_MAX];
char* argv[ARGV_MAX];
int lastcode = 0;
char pwd[1024];

char myenv[32];
char *rdirfilename = NULL;
int rdir = 3;

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

void getpwd()
{
    getcwd(pwd,sizeof(pwd));
}

char* getHostName()
{
    return getenv("HOSTNAME");
}

void check_redir(char *cmd);

void interact(int size)
{
    printf(LEFT"%s@%s %s"RIGHT"""""# ",getUsername(),getHostName(),getPwd());
    char* s = fgets(commandline,size,stdin);
    (void)s;

    assert(s);
    commandline[strlen(commandline)-1] = '\0';

    //printf("%s",commandline);
     check_redir(commandline);
}

int splitstring(char cline[], char *_argv[])
{
    int i = 0;
    argv[i++] = strtok(cline, CLINE_CUT);
    while(_argv[i++] = strtok(NULL, CLINE_CUT)); // 故意写的=
    return i - 1;
}

void check_redir(char *cmd)
{

    // ls -al -n
    // ls -al -n >/</>> filename.txt
    char *pos = cmd;
    while(*pos)
    {
        if(*pos == '>')
        {
            if(*(pos+1) == '>'){
                *pos++ = '\0';
                *pos++ = '\0';
                while(isspace(*pos)) pos++;
                rdirfilename = pos;
                rdir=2;
                break;
            }
            else{
                *pos = '\0';
                pos++;
                while(isspace(*pos)) pos++;
                rdirfilename = pos;
                rdir=1;
                break;
            }
        }
        else if(*pos == '<')
        {
            *pos = '\0'; // ls -a -l -n < filename.txt
            pos++;
            while(isspace(*pos)) pos++;
            rdirfilename = pos;
            rdir=0;
            break;
        }
        else{
            //do nothing
        }
        pos++;
    }
}


void NormalExcute(char *_argv[])
{
    pid_t id = fork();
    if(id < 0){
        perror("fork");
        return;
    }
    else if(id == 0){

        if(rdir==0)
        {
            int fd=open(rdirfilename,O_RDONLY);
            dup2(fd,0);
        }
        else if(rdir==1)//w
        {
            int fd=open(rdirfilename,O_WRONLY|O_TRUNC|O_CREAT,0666);
            dup2(fd,1);
        }
        else if(rdir==2)
        {
            int fd=open(rdirfilename,O_WRONLY|O_APPEND|O_CREAT,0666);
            dup2(fd,1);
        }

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

int buildCommand(char *_argv[], int _argc)
{
    if(_argc == 2 && strcmp(_argv[0], "cd")==0)
    {
        chdir(_argv[1]);
        getpwd();

        sprintf(getenv("PWD"),"%s",pwd);
        return 1;
    }else if(_argc==2&&strcmp(_argv[0],"export")==0){
        //导入环境变量
        strcpy(myenv,_argv[1]);
        putenv(myenv);
        printf("%s\n",myenv);
        return 1;
    }
    else if(_argc==2&&strcmp(_argv[0],"echo")==0)
    {
        if (strcmp(_argv[1],"$?")==0)
        {
            printf("%d\n",lastcode);
        }
        else if(*_argv[1]=='$')//查询
        {
            //printf("%s",getpwd())
            char* str=getenv((_argv[1]+1));
            if(str)
                printf("%s\n",str);
        }
        else{
            printf("%s\n",_argv[1]);
            //对两边 “ 的判断
        }

        return 1;
    }

    if(strcmp(_argv[0],"ls")==0)
    {
        _argv[_argc++]="--color";
        _argv[_argc]=NULL;
    }

    return 0;

}

int main()
{
    while(1)
    {
        rdirfilename = NULL;
        rdir = 3;
        interact(sizeof(commandline));
        int argc = splitstring(commandline,argv);
        if(argc == 0)
            continue;

        //内键命令
        int n=buildCommand(argv,argc);

        if(!n)
            NormalExcute(argv);
    }
    return 0;
}
