#include "EpollServer.hpp"




int main()
{
    std::shared_ptr<EpollServer> str(new EpollServer(8888));
    str->Init();
    str->run();

    return 0;
}