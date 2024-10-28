#pragma once

#include <jsoncpp/json/json.h>


//  自定义协议   "len"\n"x op y"\n


// json   fast
#define _json 1

// "len"\n
bool Decode(std::string& package)
{

}

bool Encode(std::string& package)
{

}


class Receive
{
public:
    //基础序列化
    bool Series(std::string &out)
    {
#ifdef _json
        Json::Value ser;
        ser["x"] = x;
        ser["y"] = y;
        ser["op"] = op;

        Json::FastWriter w;
        out = w.write(ser); 
#else
#endif

        return true;
    }

private:
    int x;
    int y;

    char op;
};

class Response
{
public:

private:
    int results_;
    int code_;
};