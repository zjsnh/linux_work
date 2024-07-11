#include<iostream>
#include<cstddef>
using namespace std;
#include"string.h"
#include"vector.h"



int main()
{
    vec::vector<int> V(10,2);
    for(auto e : V)
    {
        cout<< e ;
    }
    cout<<endl;


    return 0;
}
