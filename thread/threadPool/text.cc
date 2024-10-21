#include <iostream>
#include "ThreadPool.hpp"
#include "Task.hpp"


int main()
{
    ThreadPool<task> t;

    t.start();

    return 0;
}