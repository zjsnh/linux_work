#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>
#include <queue>

#define NUM 5
#define LOW NUM / 3
#define HIGH NUM * 2 / 3

template<class T>
class BlockQueue
{
    public:
        BlockQueue(int max = NUM)
            : max_(max)
        {
            pthread_mutex_init(&mutex_, NULL);
            pthread_cond_init(&com_cond_, NULL);  //消费
            pthread_cond_init(&pro_cond_, NULL);

            low_level = LOW;
            high_level = HIGH;
        }

        T pop()
        {
            
            pthread_mutex_lock(&mutex_);
            if (q_.size() == 0)
            {
                pthread_cond_wait(&com_cond_, &mutex_);
            }

            T data = q_.front();
            q_.pop();

            if(q_.size() < low_level)
            {
                pthread_cond_signal(&pro_cond_);
                cout << "商品需要补充" << endl;
                usleep(100);
            }

            pthread_mutex_unlock(&mutex_);

            return data;
        }

        void push(T data)
        {
            pthread_mutex_lock(&mutex_);

            if (q_.size() == max_)
            {
                pthread_cond_wait(&pro_cond_, &mutex_);
            }

            q_.push(data);

            if(q_.size() > high_level)
            {
                pthread_cond_signal(&com_cond_);
                cout << "商品需要被消费" << endl;
                usleep(500);
            }

            pthread_mutex_unlock(&mutex_);
        }


        ~BlockQueue()
        {
            pthread_mutex_destroy(&mutex_);
            pthread_cond_destroy(&com_cond_);
            pthread_cond_destroy(&pro_cond_);

        }

    private:
        queue<T> q_;
        pthread_mutex_t mutex_; // 锁
        pthread_cond_t com_cond_;   //队列
        pthread_cond_t pro_cond_;

        int low_level;
        int high_level;

        int max_;
};