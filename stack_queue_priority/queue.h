#include<list>
#include<vector>
#include<deque>
#include<cstddef>
namespace qu{
    template<class T, class Container=std::deque<T>>
        class queue{
            public:
                void push(const T& val)
                {
                    _con.push_front(val);
                }

                void pop()
                {
                    _con.pop_back();
                }

                const T& front()
                {
                    return _con.front();
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
