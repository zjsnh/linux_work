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

                iterator begin() const
                {
                    return _start;
                }

                iterator end() const
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

                vector(const vector<T>& V)
                {
                    reserve(V.capacity());

                    for(auto e : V)
                    {
                        push_back(e);
                    }
                }

                size_t Size() const
                {

                    return _finish-_start;
                }


                size_t capacity() const
                {
                    return _endofstorage - _start;
                }


                ~vector()
                {
                    delete[] _start;

                    _start=nullptr;
                    _finish=nullptr;
                    _endofstorage=nullptr;
                }

                void swap(vector<T>& v)
                {
                    std::swap(_start,v._start);
                    std::swap(_finish,v._finish);
                    std::swap(_endofstorage,v._endofstorage);
                }


                vector<T>& operator=(vector<T> v)
                {
                    swap(v);

                    return *this;
                }


                void push_back(T val)
                {
                    if(Size()==capacity())
                    {
                        reserve(capacity()==0?4:2*capacity());
                    }

                   *(_finish)=val;
                   _finish++;
                }

                void resize(size_t size,const T& val=T())
                {
                    if(size<=Size())
                    {
                        _finish=_start+size;
                    }
                    else{
                        reserve(size);
                        while(_finish!=_start+size)
                        {
                            *_finish=val;
                            _finish++;
                        }
                    }
                }

                void insert(iterator pos,const T& val)
                {
                    assert(pos>=_start);
                    assert(pos<_finish);

                    if(Size()==capacity())
                    {
                        size_t len=pos - _start;
                        reserve(capacity()==0?4:2*capacity());

                        pos=_start+len; //防止扩容后pos位置改变 因为是指针
                    }

                    memmove(pos+1,pos,sizeof(int)*(_finish-pos));

                    *pos=val;
                    _finish++;
                }

                iterator erase(iterator pos)
                {
                    assert(pos>=_start);
                    assert(pos<_finish);

                    memmove(pos,pos+1,sizeof(int)*(_finish-pos));
                    _finish--;

                    return pos; 
                }

                T& operator[](size_t pos)
                {
                    assert(pos<Size());

                    return _start[pos];
                }

                const T& operator[](size_t pos) const
                {
                    assert(pos<Size());

                    return _start[pos];
                }


            private:
                iterator _start = nullptr;
                iterator _finish = nullptr;
                iterator _endofstorage = nullptr;
        };
}
