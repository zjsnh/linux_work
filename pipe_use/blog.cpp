#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
using namespace std;


#define PWD_CUT "/"

char* Username()
{
    return getenv("USER");
}

char* getPwd()
{
    char *pwd = getenv("PWD");
    char *token = strtok(pwd, PWD_CUT);

    char *last_token;
    while (token != NULL)
    {
        last_token = token;
        token = strtok(NULL, PWD_CUT);
    }

    return last_token;
}

char* HostName()
{
    return getenv("HOSTNAME");
}

int main()
{

    printf("["
           "%s@%s %s"
           "]"
           "#",
           Username(), HostName(), getPwd());

    cout << endl;

    return 0;
}