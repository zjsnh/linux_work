#pragma once 


#include<cstdbool>
#include<cassert>
#include<cstring>
#include<utility>

namespace str
{
    class string
    {
        public:
            string(const char* str="")
                :_size(strlen(str))
            {
                _capacity=_size;
                _str=new char[_capacity+1];
                strcpy(_str,str);

            }

            void swap(string& temp)
            {
                std::swap(_str,temp._str);
                std::swap(_size,temp._size);
                std::swap(_capacity,temp._capacity);
            }

            string(const string& S)
                :_str(nullptr)
                 ,_size(0)
                 ,_capacity(0)
            {
                string temp(S._str);
                swap(temp);
            }

            ~string()
            {
                delete[] _str;
                _size=0;
                _capacity=0;
                _str=nullptr;
            }

            typedef char* iterator;
            typedef const char* const_iterator;

            iterator begin()
            {
                return _str;
            }

            iterator end()
            {
                return _str+_size;
            }

            void reserve(size_t n)
            {
                if(n>_capacity)
                {
                    char* temp=new char[n+1];
                    strcpy(temp,_str);
                    delete[] _str;
                    _str=temp;
                    _capacity=n;
                }
            }

            void push_back(char c)
            {
                if(_size==_capacity)
                {
                    reserve(_capacity==0?4:_capacity*2);
                }

                _str[_size++]=c;
                _str[_size]='\0';
            }


            void append(const char* str)
            {
                size_t size=strlen(str);
                if(_size+size>=_capacity)
                {
                    reserve(_size+size);
                }

                strcpy(_str+_size,str);
                _size+=size;

            }
            


        private:
            char* _str;
            size_t _size;
            size_t _capacity;

            const static size_t npos=-1;

    };

};
