#include "BloackQueue.hpp"


void* consumer(void *args)
{
    pthread_detach(pthread_self());
    pair<BlockQueue<int> *, int> *q = static_cast<pair<BlockQueue<int> *, int> *>(args);

    while(true)
    {
        usleep(10000);

        int data = (*(*q).first).pop();

        cout << "商品" << data << "被" << (*q).second << "消费" << endl;
    }
}


void* producer(void *args)
{
    pthread_detach(pthread_self());
    pair<BlockQueue<int> *, int> *q = static_cast<pair<BlockQueue<int> *, int> *>(args);

    int data = 1;

    while(true)
    {
        usleep(10000);

        (*(*q).first) .push(data);
        cout << "商品" << data << "被" << (*q).second << "生产" << endl;
        data++;
    }
}


int main()
{
    pthread_t c[5];
    pthread_t p[3];

    BlockQueue<int> *_text = new BlockQueue<int>;

    

    for (int i = 0; i < 5; i++)
    {
        pair<BlockQueue<int> *, int> *text = new pair<BlockQueue<int> *, int>(_text, i);
        pthread_create(&c[i], NULL, consumer, text);
    }

    for (int j = 0; j < 3; j++)
    {
        pair<BlockQueue<int> *, int> *text = new pair<BlockQueue<int> *, int>(_text, j);
        pthread_create(&p[j], NULL, producer, text);
    }
        


    while(true)
    {}

    return 0;
}
