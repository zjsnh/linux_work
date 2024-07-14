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


    template<class T,class Ref,class Ptr>
        struct __List_Iterator
        {
            typedef ListNode<T> Node;
            typedef __List_Iterator<T,Ref,Ptr> sef;
            typedef __List_Iterator<T,T&,T*> iterator;

            typedef Ref reference;
            typedef Ptr pointer;

            Node* _node;

            __List_Iterator(Node* node)
                :_node(node)
            {}

            __List_Iterator(const sef& it)
                :_node(it._node)
            {}

            bool operator==(const sef& it)
            {
                return _node==it._node;
            }

            bool operator!=(const sef& it)
            {
                return _node!=it._node;
            }

            reference operator*() const{
                return _node->_data;
            }

            pointer operator->() const{
                //return &(_node->_data);
                
                return &operator*();
            }

            sef& operator++()
            {
                _node=_node->_next;

                return *this;
            }

            sef& operator--()
            {
                _node=_node->_prev;

                return *this;
            }



        };

    template<class T>
        class list{

            public:
                typedef ListNode<T> Node;
                typedef __List_Iterator<T,T&,T*> iterator;


                iterator begin()
                {
                    return _head->_next;
                }


                iterator end()
                {
                    return _head;
                }

                iterator begin() const{
                    return _head->_next;
                }

                iterator end() const{
                    return _head;
                }

                list()
                {
                    empty();
                }

                list(const list& l)
                {
                    empty();

                    for(auto e : l)
                    {
                        push_back(e);
                    }
                }

                ~list()
                {
                    clear();
                }

                void clear()
                {
                    iterator it =begin();
                    while(it!=end())
                    {
                        it=erase(it);
                    }
                }


                void empty()
                {
                    _head=new Node;
                    _head->_next=_head;
                    _head->_prev=_head;

                    ++_size;

                }

                void push_back(const T& val )
                {
                    insert(end(),val);
                }

                void pop_back()
                {
                    erase(--end());
                }

                void pop_front()
                {
                    erase(begin());
                }

                iterator insert(iterator pos , const T& val)
                {
                    Node* temp=pos._node;            

                    Node* newnode = new Node(val);
                    Node* prev=temp->_prev;


                    prev->_next=newnode;
                    newnode->_prev=prev;

                    newnode->_next=temp;
                    temp->_prev=newnode;

                    ++_size;

                    //return newnode;
                    return iterator(newnode);
                }

                iterator erase(iterator pos)
                {
                    Node* cur=pos._node;
                    Node* prev=cur->_prev;
                    Node* next=cur->_next;

                    delete cur;

                    prev->_next=next;
                    next->_prev =prev;

                    --_size;

                    return iterator(next);
                }
            private:
                Node* _head;
                size_t _size;
        };

}
