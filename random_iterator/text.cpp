#include<iostream>
#include<cstddef>
#include<cstring>
#include<utility>
using namespace std;
#include"string.h"




int main()
{


    str::string S("xx");

    str::string::iterator it=S.begin();
    while(it!=S.end())
    {
        cout<<*it;
        it++;
    }
    cout<<endl;

    S.append("NNXXX");

    it=S.begin();
    while(it!=S.end())
    {
        cout<<*it;
        it++;
    }
    cout<<endl;


    return 0;
}
