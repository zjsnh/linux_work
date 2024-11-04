#pragma once

#include <jsoncpp/json/json.h>
#include <string>
#include <cstdlib>

std::string separator = "\n";

//  自定义协议   "len"\n"x op y"\n


// json   fast
//#define _json 0

// "len"\n
bool Decode(std::string& package)
{
    size_t len = package.size();

    std::string temp = std::to_string(len);
    temp += separator;

    temp += package;
    temp += separator;
    package = temp;

    return true;
}

//"len"\n"x op y"\n
bool Encode(std::string& package,std::string& content)
{
    size_t left = package.find_first_of(separator);
    if (left == std::string::npos)
        return false;

    std::string _size_len = package.substr(0, left);
    size_t _size = atoi(_size_len.c_str());

    size_t total_len = _size_len.size() + _size + 2;
    if(package.size() < total_len)
        return false;

    content = package.substr(left + 1, total_len);
    package.erase(0, total_len);

    return true;
}

class Request
{
public:
    Request(int data1, int data2, char oper) : x(data1), y(data2), op(oper)
    {
    }
    Request()
    {}
public:
    //基础序列化  "x + y"
    bool Serialize(std::string &out)
    {
        std::string temp = std::to_string(x);
        temp += " ";
        temp += op;
        temp += " ";
        temp += std::to_string(y);
        

        out = temp;

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


    bool Deserialize(std::string& in)
    {
        size_t left = in.find_first_of(" ");
        if(left == std::string::npos)
            return false;

        std::string part_left = in.substr(0, left);

        size_t right = in.rfind(" ");
        if(right == std::string::npos)
            return false;

        if (left + 2 != right)
            return false;

        std::string part_right = in.substr(right + 1);

        x = atoi(part_left.c_str());
        y = atoi(part_right.c_str());

        op = in[left + 1];

        return true;
    }

public:
    int x;
    int y;

    char op;
};

class Response
{

public:
    Response(int res, int c) : results_(res), code_(c)
    {
    }

    Response()
    {}

public:   //"results_ code_"
    bool Serialize(std::string &out)
    {
        std::string temp = std::to_string(results_);
        temp += " ";
        temp += std::to_string(code_);

        out = temp;

        return true;
    }


    bool Deserialize(std::string& in)
    {
        size_t left = in.find_first_of(" ");
        if (left == std::string::npos)
            return false;
        std::string part_left = in.substr(0, left);

        std::string part_right = in.substr(left + 1);
        results_ = atoi(part_left.c_str());
        code_ = atoi(part_right.c_str());

        return true;
    }

public:
    int results_;
    int code_;
};