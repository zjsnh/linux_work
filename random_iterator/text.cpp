#include<iostream>
#include<cstddef>
using namespace std;
#include"string.h"
#include"vector.h"
#include"list.h"


int main()
{

    lt::list<int> l;
    l.push_back(1);

    l.push_back(1);
    l.push_back(4);
    l.push_back(3);
    l.push_back(2);
    l.push_back(1);
    l.push_back(8);


    for(auto e:l)
    {
        cout<<e<<" ";
    }
    cout<<endl;

    lt::list<int> l2(l);


    for(auto e:l2)
    {
        cout<<e<<" ";
    }
    cout<<endl;


    l2.pop_back();

    l2.pop_back();
    l2.pop_back();
    l2.pop_back();
    l2.pop_front();
    for(auto e : l2)
    {
        cout<<e<<" ";
    }
    cout<<endl;




    return 0;
}

