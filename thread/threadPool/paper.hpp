#pragma once



#include <iostream>
#include <pthread.h>
#include <vector>
#include <queue>


namespace paper
{
    #define NUM 5

    struct threads_
    {
        pthread_t tid;
    };

    template<class T>  //任务结构体
    class pthread_pool
    {
    private:

        pthread_pool(int num)
            :threads_(num)
        {
            pthread_mutex_init(&_mutex, NULL);
            pthread_cond_init(&_cond, NULL);
        }

        ~pthread_pool()
        {
            pthread_mutex_destroy(&_mutex);
            pthread_cond_destroy(&_cond);
        }

        pthread_pool<T>(const pthread_pool &p) = delete;

        const pthread_pool<T>& operator=(const pthread_pool &p) = delete;

    public:
    static void* Hand_task(void* args)
    {
        pthread_detach(pthread_self());
        pthread_pool *this_ = static_cast<pthread_pool *>(args);

        while(true)
        {
            pthread_mutex_lock(&_mutex);
            while((this_->task_).empty())
            {
                pthread_cond_wait(&_cond);
            }

            this_->Pop();
            pthread_mutex_unlock(&_mutex);

            //任务处理
        }
    }

    void start()
    {
        int num = Thread_.size();
        for (int i = 0; i < num; i++)
        {
            pthread_create(&(Thread_[i].tid), NULL, Hand_task, this);
        }
        return;
    }

    void Push(const T& task)
    {
        pthread_mutex_lock(&_mutex);
        task.push(task);

        pthread_cond_signal(&_cond);
        pthread_mutex_unlock(&_mutex);
    }

    T Pop()
    {
        T task;
        task = task.top();
        task.pop();

        return task;
    }


    //接口函数  GetIn

    static pthread_pool<T>*& Get(size_t num = NUM)
    {
        if(tp == nullptr)
        {
            pthread_mutex_lock(pmutex);
            while (tp == nullptr)
            {
                tp = new pthread_pool<T>(num);
            }
            pthread_mutex_unlock(pmutex);
        }
        return tp;
    }

    //拷贝构造  赋值重载 delete



    private :
        std::vector<threads_> Thread_;
        std::queue<T> task_;

        pthread_mutex_t _mutex;
        pthread_cond_t _cond;

        static pthread_pool<T> *tp;
        static pthread_mutex_t pmutex;
    };

    template <class T>
    pthread_pool<T> *pthread_pool<T>::tp = nullptr;

    template <class T>
    pthread_mutex_t pthread_pool<T>::pmutex = PTHREAD_MUTEX_INITIALIZER;
}