#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
using namespace std;
#include "sem.hpp"

void* consumer(void* args){

    pair<Ring_queue<int> *, int> *_p = static_cast<pair<Ring_queue<int> *, int> *>(args);

    while(true)
    {
        int data = 0;
        (_p->first)->pop(data);

        cout << "消费 " << data << "from consumer thread:" << _p->second << endl;

        usleep(30000);
    }

    return NULL;
}

void* producer(void* args){

    sleep(2);

    pair<Ring_queue<int> *, int> *_p = static_cast<pair<Ring_queue<int> *, int> *>(args);

    while(true)
    {
        int data = rand() % 100;

        _p->first->push(data);
        cout << "生产 " << data << "from producer thread:" << _p->second << endl;
        usleep(30000);
    }

    return NULL;
}

int main()
{
    srand(time(NULL));
    pthread_t c[3], p[5];

    Ring_queue<int> *rq = new Ring_queue<int>;

    for (int i = 0; i < 3; i++)
    {
        pair<Ring_queue<int> *, int>* _p = new pair<Ring_queue<int> *, int>(rq, i);
        pthread_create(c + i, NULL, consumer, _p);
    }

    for (int i = 0; i < 5; i++)
    {
        pair<Ring_queue<int> *, int>* _p = new pair<Ring_queue<int> *, int>(rq, i);
        pthread_create(p + i, NULL, producer,_p);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(p[i], NULL);
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(c[i], NULL);
    }

    return 0;
}