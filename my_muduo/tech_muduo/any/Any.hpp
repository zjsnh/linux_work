#pragma once
#include <string>
#include <typeinfo>
#include <cassert>
#include <algorithm>

class Any
{
private:
    class holder
    {
    public:
        virtual ~holder() {}
        virtual const std::type_info &type() = 0;
        virtual holder *clone() = 0;
    };

    template<class T>
    class placeholder : public holder
    {
    public:
        placeholder(const T &val) : _val(val) {}

        const std::type_info& type()
        {
            return typeid(_val);
        }

        holder* clone()
        {
            return new placeholder(_val);
        }

    public:
        T _val;
    };

    holder *_content; //子类进行类型转换，访问的函数仍然是子类函数  多态

    void swap(Any& any)
    {
        std::swap(_content, any._content);  //地址交换
        return;
    }

public:
    Any():_content(nullptr) {}  //需要初始化
    template <class T>
    Any(const T &val) : _content(new placeholder<T>(val)){}
    Any(const Any &any) : _content(any._content ? any._content->clone() : NULL) {}

    ~Any() { delete _content; }

    template <class T>
    T* Get()
    {
        assert(typeid(T) == _content->type());

        return &((placeholder<T> *)_content)->_val;
    }

    template<class T>
    Any& operator=(const Any&any)
    {
        Any temp(any);
        swap(temp);

        return *this;
    }

    template<class T>
    Any& operator=(const T&val)
    {
        Any temp(val);
        swap(temp);

        return *this;
    }
};