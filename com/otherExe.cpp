#include<iostream>
using namespace std;

#include<unistd.h>

int main(int argc,char* argv[], char* env[])
{
    cout<<argv[0]<<endl;

    for(int i=0;argv[i];i++)
    {
        cout<<argv[i]<<endl;
    }

    for(int i=0;env[i];i++)
    {
        cout<<env[i]<<endl;
    }


    return 0;
}
