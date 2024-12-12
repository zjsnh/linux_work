#include "SelectServer.hpp"
#include <iostream>
#include <memory>
#include <sys/select.h>
#include <stdio.h>
#include <thread>

#include <unistd.h>  // 为了使用 sleep 函数模拟输入延时

void* example_select(void* args) {
    fd_set read_fds;
    FD_ZERO(&read_fds); // 初始化描述符集
    FD_SET(0, &read_fds); // 添加标准输入（文件描述符 0）
    struct timeval timeout = {5, 0}; // 超时时间 5 秒

    // 使用 select 来监视标准输入
    int ret = select(1, &read_fds, NULL, NULL, &timeout);
    if (ret > 0 && FD_ISSET(0, &read_fds)) {
        std::cout << timeout.tv_sec << std::endl;
        printf("Input is available.\n");
    } else if (ret == 0) {
        printf("Timeout occurred.\n");
    } else {
        perror("select error");
    }

    return NULL;
}

void simulate_input() {
    std::cout << "Simulating input..." << std::endl;
    std::cin.get();  
}

int main() {
    // 创建两个线程
    std::thread monitor_thread(example_select, nullptr);  // 监视输入线程
    std::thread input_thread(simulate_input);  // 模拟输入线程


    monitor_thread.join();
    input_thread.join();

    return 0;
}

// int main(int argc,char* argv[])
// {

    

//     // std::cout << argv[1] << std::endl;
//     // std::unique_ptr<SelectServer> ptr(new SelectServer((uint16_t)atoi(argv[1])));

//     // ptr->Init();
//     // ptr->Run();

//     std::cout << sizeof(fd_set)*8 << std::endl;

//     return 0;
// }