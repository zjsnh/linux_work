#pragma once 

#include<cstddef>
#include<cstdbool>


namespace lt
{
    template<class T>
        struct ListNode{
            T _data;
            ListNode* _next;
            ListNode* _prev;


            ListNode(const T& val =T())
            :_data(val),
            _next(nullptr),
            _prev(nullptr)
            {}
        };

    template<class T>
        class list{
            public:
                list()
                {

                }

                list(const list& l)
                {

                }
            private:
                T* _head;
                size_t size;
        };

}
