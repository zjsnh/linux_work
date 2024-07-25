#include<deque>
#include<cstddef>
#include<vector>

template<class T>
struct less
{
    bool operator()(T p1,T p2)
    {
        return p1>p2;
    
};


namespace heap{
    template<class T,class Container=std::vector<T> ,class compare=less<T>>
        class heap{
            public:

            void adjust_up()
            {
                compare com;
                int child=_con.size()-1;
                int parent=(child-1)/2;

                while(child>0)
                {
                    if(com(_con[child],_con[parent]))
                    {
                        std::swap(_con[child],_con[parent]);
                        child=parent;
                        parent=(child-1)/2;

                    }
                    else{
                        break;
                    }
                }
            }

            void push(const T& val)
            {
                _con.push_back(val);
                adjust_up();
            }

            private:
            Container _con;
        };
}
