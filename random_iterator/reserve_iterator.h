#pragma once 


template <class Iterator , class Ref , class Ptr>
class ReverseIterator
{
    typedef ReverseIterator<Iterator , Ref , Ptr> sef;

    public:
        ReverseIterator(Iterator it)
            :_it(it)
        {}

        ReverseIterator(const sef& it)
            :_it(it._it)
        {}

        sef& operator++()
        {
            --_it;
            return *this;
        }

        sef& operator--()
        {
            ++_it;
            return *this;
        }

        Ref operator*() const
        {
            Iterator temp(_it);
            return *(--temp);
        }

        Ptr operator->() const{
            return &(operator*());
            //return _it->operator->();
        }

        bool operator==(const sef& it)
        {
            return _it==it._it;
        }

        bool operator!=(const sef& it)
        {
            return _it!=it._it;
        }

    private:
        Iterator _it;
};
