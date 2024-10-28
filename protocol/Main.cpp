#include <iostream>
#include "socket.hpp"




int main()
{
    Socket _socket;
    _socket.Bind();
    _socket.Listen();

    return 0;
}