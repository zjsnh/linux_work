#pragma once

#include <pthread.h>

#include <iostream>
#include <vector>
#include <queue>
#include <string>

#define NUM 10

struct threads_
{
    pthread_t tid_;
    std::string name_;
};


//

template<class T>
class ThreadPool
{

    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    void pro_sleep()
    {
        pthread_cond_wait(&cond_,&mutex_);
    }

    void wakeup()
    {
        pthread_cond_signal(&cond_);
    }

public:

    static void* Handle_Task(void *args)   //静态函数，
    {
        pthread_detach(pthread_self());

        ThreadPool<T> *this_ = static_cast<ThreadPool<T> *>(args);

        while(true)
        {
            this_->lock();
            while((this_->task_).empty())
            {
                this_->pro_sleep();
            }

            T task = this_->pop();
            this_->unlock();

            task();  //处理
        }
    }


    void start()
    {
        int num = Thread_.size();
        for (int i = 0; i < num; i++)
        {
            pthread_create(&(Thread_[i].tid_), NULL, Handle_Task, this); 
        }
    }


    void Push(const T& task)
    {
        lock();
        task_.push(task);
        wakeup();

        unlock();
    }

    T pop()
    {
        T task = task_.front();
        task_.pop();

        return task;
    }

    static ThreadPool<T>*& Get_pointer()
    {
        if(tp == NULL)  //如果 tp已经存在不会进行上锁
        {
            pthread_mutex_lock(&static_mutex);
            if (tp == NULL)
            {
                tp = new ThreadPool<T>();
            }

            pthread_mutex_unlock(&static_mutex);
        }

        return tp;
    }

    

private:
    ThreadPool(size_t num = NUM)
        :Thread_(num)
    {
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&cond_, NULL);
    }


    ~ThreadPool()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }

    ThreadPool(const ThreadPool<T> &) = delete;
    const ThreadPool<T> &operator=(const ThreadPool<T> &) = delete; 
private:

    std::vector<threads_> Thread_;
    std::queue<T> task_;

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    static ThreadPool<T> *tp;
    static pthread_mutex_t static_mutex;
};

template <class T>
pthread_mutex_t ThreadPool<T>::static_mutex = PTHREAD_MUTEX_INITIALIZER;

template <class T>
ThreadPool<T>* ThreadPool<T>::tp = NULL;