#pragma once

/*所有任务都需要任务结构体，并包含
相关函数
    class Task
    {
    public:
        const string& name();
        void opreator()(); //如何执行

        ...

    }
*/

#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <queue>

static int num_ = 5;

class Thread_Info
{
    pthread_t tid;
    std::string name;
};

template<class T>
class Thread_Pool
{
public:
    const string& Thread_Name(Thread_t _tid)
    {
        for(auto& e:thread_)
        {
            if(e.tid==_tid)
            {
                return e.name;
            }
        }

        retrun "NONE";
    }

    Thread_Pool(int num = num_)
        : thread_(num),
    {
        pthread_mutex_init(&mutex_);
        pthread_cond_init(&cond_,NULL);
    }

    static void* Thread_Task(void* args)
    {
        Thread_Pool<T> *this_ = static_cast<Thread_Pool<T> *> args;

        while(true)
        {
            pthread_mutex_lock(&mutex_);
            while(thread_task_.empty())
            {
                pthread_cond_wait(&cond_, &mutex_);
            }

            T task = this_->Thread_Task_Pop;
            cout << this_->Thread_Name << "get Taks: " << task.name() << endl;

            pthread_mutex_unlock(&mutex_);
            task();
        }
    }

    void Thread_Run()
    {
        for (int i = 0; i < thread_.size(); i++)
        {
            thread_[i].name = "thread-" + to_string(i);
            pthread_create(thread_[i].tid, NULL, Thread_Task, this);
        }
    }

    void Thread_Task_Push(const T& task)
    {
        pthread_mutex_lock(&mutex_);
        thread_task_.push(task);
        pthread_cond_signal(&cond_);
        pthread_mutex_unlock(&mutex_);
    }

    T Thread_Task_Pop()
    {
        T task = thread_task_.front();
        thread_task_.pop();

        return task;
    }

    ~Thread_Pool()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }



private:
    std::vector<Thread_Info> thread_;
    std::queue<T> thread_task_;

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
};