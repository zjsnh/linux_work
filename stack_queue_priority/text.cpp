#include<iostream>
#include<cstddef>
using namespace std;
#include<unistd.h>
#include"stack.h"



int main(){

    std::deque<int>::iterator it;

    sk::stack<int> S;
    S.push(1);
    S.push(2);
    S.push(1);
    S.push(9);
    S.push(8);
    S.push(7);
    S.push(3);
    S.push(2);

    for (auto e : S)
    {
	    cout << e << endl;
    }

    return 0;
}
