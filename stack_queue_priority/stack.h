#include<list>
#include<vector>
#include<deque>
#include<cstddef>

namespace sk{
    template<class T, class Container=std::deque<T>>
        class stack{
            public:

                typedef typename std::deque<T>::iterator iterator;

                iterator begin()
                {
                    return _con.begin();
                }

                iterator end()
                {
                    return _con.end();
                }


                void push(const T& val)
                {
                    _con.push_back(val);
                }

                void pop()
                {
                    _con.pop_back();
                }

                const T& top()
                {
                    _con.back();
                }

                bool empty()
                {
                    return _con.empty();
                }

                size_t size()
                {
                    return _con.size();
                }
            private:
                Container _con;
        };
}

