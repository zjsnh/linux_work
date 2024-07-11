#pragma once 

#include<cassert>
#include<cstdbool>
#include<utility>
#include<cstddef>
namespace vec 
{
    template<class T>
        class vector{
            public:
                typedef T* iterator;
                typedef const T* const_iterator;

                iterator begin()
                {
                    return _start;
                }

                iterator end()
                {
                    return _finish;
                }

                void reserve(size_t n)
                {
                    size_t _capacity=_endofstorage-_start;
                    if(n > _capacity)
                    {

                        size_t size=_finish-_start;

                        T* temp=new T[n];
                        for(size_t i=0;i<size;i++)
                        {
                            temp[i]=_start[i];
                        }

                        delete[] _start;
                        _start = temp;
                        _finish = _start + size;
                        _endofstorage = _start + n;

                    }

                }

                vector()
                {}

                vector(size_t size,const T&val =T())
                {
                    reserve(size);
                    while(_finish!=_endofstorage)
                    {
                        *_finish=val;
                        _finish++;
                    }
                }

                void swap(vector<T> V)
                {

                }

                vector(const vector<T>& V)
                {
                    reserve(V.capacity());

                    for(auto e : V)
                    {
                        push_back(e);
                    }
                }

                size_t capacity()
                {
                    return _endofstorage - _start;
                }

            private:
                iterator _start = nullptr;
                iterator _finish = nullptr;
                iterator _endofstorage = nullptr;
        };
}
