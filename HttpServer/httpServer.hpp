#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <unordered_map>
#include "socket.hpp"

static std::string Row_segmentation = "\r\n";
static std::string wwwroot = "./wwwroot";
static std::string homepage = "index.html";

class HttpServer;

class thread_msg
{
public:
    thread_msg(int socket , HttpServer *_this)
        :socket_(socket) , svr(_this)
    {
    }

public:
    int socket_;
    HttpServer *svr;
};

class HttpRequest
{
public:
    void Deserialize(std::string& in)
    {
        while (true)
        {
            ssize_t pos = in.find(Row_segmentation);
            if(pos == std::string::npos)
                break;
            
            std::string row = in.substr(0, pos);
            if(row.empty())
                break;
            

            in.erase(0, pos + Row_segmentation.size());
            row_.push_back(row);
        }
    }

    void Req_Debug()
    {
        std::cout << "Req_Debug running" << endl;
        for(auto&e : row_)
        {
            std::cout << e << std::endl;
        }
    }

    void Prase()
    {
        std::stringstream ss(row_[0]);
        ss >> mothed >> url >> http_version;
        file_path = wwwroot;

        if(url == "/" || url == "/index.html") {
            file_path += "/";   //  ./wwwroot
            file_path += homepage; // ./wwwroot/index.html
        }
        else
            file_path += url;

        ssize_t pos = file_path.rfind(".");
        if(pos == std::string::npos)
            suffix = ".html";
        else
            suffix = file_path.substr(pos);
    }

public:
    std::vector<std::string> row_;

    std::string mothed;
    std::string url;
    std::string http_version;
    std::string file_path;

    std::string suffix;
};

class HttpServer
{
public:
    //socket
    HttpServer(u_int16_t port)
        :port_(port)
    {
        content_type.insert({".html", "text/html"});
        content_type.insert({".png", "image/png"});
    }

    std::string ReadHtmlContent(const std::string& file_path)
    {
        //二进制读写
        std::ifstream in(file_path, std::ios::binary);
        if(!in.is_open()) return "";

        in.seekg(0, std::ios_base::end);
        auto len = in.tellg();
        in.seekg(0, std::ios_base::beg);

        std::string content;
        content.resize(len);

        in.read((char*)content.c_str(), content.size());

        in.close();

        return content;
    }

    std::string SuffixToDesc(std::string suffix)
    {
        return content_type[suffix];
    }

    void HandlerHttp(int sockfd)
    {
        std::string stream_buffer;
        char inbuffer[10240];

        ssize_t n = recv(sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
        if(n > 0)
        {
            inbuffer[n] = 0;
            std::string in = inbuffer;
            // 反序列化
            HttpRequest req;
            req.Deserialize(in);
            req.Req_Debug();
            req.Prase();

            //转发  

            std::string text;
            text = ReadHtmlContent(req.file_path);

           // 重定向
            std::string stream_buffer = "HTTP/1.0 302 Found\r\n";
            stream_buffer += "Location: https://join.qq.com/post_detail.html?pid=2&id=101&tid=2\r\n"; // 需要添加冒号
            stream_buffer += "Content-Length: 0\r\n"; // 没有实际内容
            stream_buffer += "Connection: close\r\n"; // 关闭连接
            stream_buffer += "\r\n"; // 空行表示头部结束

            ssize_t n = send(sockfd, stream_buffer.c_str(), stream_buffer.size(), 0);
            // stream_buffer = "HTTP/1.0 200 OK\r\n";

            // std::string response_header = "Content-Length: ";
            // response_header += std::to_string(text.size()); // Content-Length: 11
            // response_header += "\r\n";
            // response_header += "Content-Type: ";
            // response_header += SuffixToDesc(req.suffix);
            // response_header += "\r\n";

            // std::string blank_line = "\r\n"; // \n

            // stream_buffer += response_header;
            // stream_buffer += blank_line;
            // stream_buffer += text;

            // ssize_t n = send(sockfd, stream_buffer.c_str(), stream_buffer.size(), 0);

        }
        
        close(sockfd);
    }

    static void *handle(void* args)
    {
        pthread_detach(pthread_self());
        thread_msg *Ser = static_cast<thread_msg*>(args);
        Ser->svr->HandlerHttp(Ser->socket_);
        delete Ser;

        return nullptr;
    }

    void Init()
    {
        _socket._socket();
        _socket.Bind(port_);
        _socket.Listen();

        std::cout << "server init success!" << std::endl;
    }

    void Start()
    {
        while(true)
        {
            struct sockaddr_in client;

            int sockfd = _socket.Accept(client);
            if(sockfd < 0)
                continue;

            _log(Info, "accept a new link, sockfd: %d", sockfd);

            pthread_t pid;
            thread_msg *msg  = new thread_msg(sockfd, this);
            pthread_create(&pid, NULL, handle, (void*)(msg));
        }
    }


private:
    u_int16_t port_;
    Socket _socket;

    std::unordered_map<std::string, std::string> content_type;
};