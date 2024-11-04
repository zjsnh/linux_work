#pragma once
#include "protocol.hpp"
#include <string.h>

enum
{
    Div_Zero = 1,
    Mod_Zero,
    Other_Oper
};

class calculator
{
public:
    Response CalculatorHelper(const Request &req)
    {
        Response resp(0, 0);
        switch (req.op)
        {
        case '+':
            resp.results_ = req.x + req.y;
            break;
        case '-':
            resp.results_ = req.x - req.y;
            break;
        case '*':
            resp.results_ = req.x * req.y;
            break;
        case '/':
        {
            if (req.y == 0)
                resp.code_ = Div_Zero;
            else
                resp.results_ = req.x / req.y;
        }
        break;
        case '%':
        {
            if (req.y == 0)
                resp.code_ = Mod_Zero;
            else
                resp.results_ = req.x % req.y;
        }
        break;
        default:
            resp.code_ = Other_Oper;
            break;
        }

        return resp;
    }


    std::string Calculator(std::string &package)
    {
        std::string content;
        if(!Encode(package, content))
            return "";

        Request req;
        if(!req.Deserialize(content))
            return "";

        content.clear();
        Response res = CalculatorHelper(req);
        res.Serialize(content);
        Decode(content);
        return content;
    }
};