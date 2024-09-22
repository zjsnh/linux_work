#pragma once
#include<iostream>
#include<functional>
#include<unordered_map>


void task1()
{
    std::cout << "task1" << std::endl;
}

void task2()
{
    std::cout << "task2" << std::endl;
}

void task3()
{
    std::cout << "task3" << std::endl;
}

void task4()
{
    std::cout << "task4" << std::endl;
}

void task5()
{
    std::cout << "task5" << std::endl;
}

function<void()> t1 = task1;
function<void()> t2 = task2;
function<void()> t3 = task3;
function<void()> t4 = task4;
function<void()> t5 = task5;

unordered_map<int, function<void()>> task_list = {
    {1, t1},
    {2, t2},
    {3, t3},
    {4, t4},
    {5, t5}
};