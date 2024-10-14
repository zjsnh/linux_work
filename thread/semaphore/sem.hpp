#pragma once

#include <iostream>
#include <vector>
#include <semaphore.h>
#include <pthread.h>

#define NUM 10

template<class T>
class Ring_queue
{
    private:
        // bool P(sem_t* sem) {
        //     if (sem_wait(sem) == -1) {
        //         perror("sem_wait failed");
        //         return false;
        //     }
        //     return true;
        // }

        // bool V(sem_t* sem) {
        //     if (sem_post(sem) == -1) {
        //         perror("sem_post failed");
        //         return false;
        //     }
        //     return true;
        // }

        void P(sem_t &sem)
        {
            sem_wait(&sem);
        }

        void V(sem_t &sem)
        {
            sem_post(&sem);
        }


    public:
        Ring_queue(int space_size = NUM)
        :queue_(space_size) ,cap_(space_size), space_pos_(0), data_pos_(0)
        {
            sem_init(&com_sem_, 0, 0);
            sem_init(&pro_sem_, 0, space_size);

            pthread_mutex_init(&pro_mutex_, NULL);
            pthread_mutex_init(&com_mutex_, NULL);

            // pthread_cond_init(&pro_cond_, nullptr);
            // pthread_cond_init(&com_cond_, nullptr);

        }

        void pop(T& data) 
        {
            P(com_sem_);
            pthread_mutex_lock(&com_mutex_);

            data = queue_[data_pos_];
            data_pos_++;
            data_pos_ %= cap_;
            pthread_mutex_unlock(&com_mutex_);
            V(pro_sem_);

            
        }

        void push(const T& data)
        {
            //获取data数据
            P(pro_sem_);
            pthread_mutex_lock(&pro_mutex_);

            queue_[space_pos_] = data;
            space_pos_++;
            space_pos_ %= cap_;

            pthread_mutex_unlock(&pro_mutex_);
            V(com_sem_);

        }

        ~Ring_queue()
        {
            sem_destroy(&com_sem_);
            sem_destroy(&pro_sem_);

            pthread_mutex_destroy(&pro_mutex_);
            pthread_mutex_destroy(&com_mutex_);
        }

    private:
        std::vector<T> queue_;
        sem_t com_sem_;
        sem_t pro_sem_;

        int cap_;
        int space_pos_;
        int data_pos_;

        pthread_mutex_t pro_mutex_;  //生产者互斥锁
        pthread_mutex_t com_mutex_;  //消费者互斥锁

        // pthread_cond_t pro_cond_;
        // pthread_cond_t com_cond_;
};
