#pragma once


#include "util.hpp"
#include "sql.h"

namespace Cloud
{
    class Usr
    {
    public:
        static bool verify(std::string username,std::string password)
        {
            //std::cout << password << std::endl;
            MYSQL *mysql = sql::GetInstance();
            std::string query = "select * from Users where UserName = '" + username + "'";
            if (mysql_query(mysql, query.c_str())) {
                std::cerr << "Query failed: " << mysql_error(mysql) << "\n";
                return false;
            }

            MYSQL_RES* result = mysql_store_result(mysql);
            if (!result) {
                std::cerr << "Result storing failed: " << mysql_error(mysql) << "\n";
                return false;
            }

            if(mysql_num_rows(result) > 0)
            {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row != NULL && row[1] == password)
                {
                    std::cout << "登录完成" << std::endl;
                    return true;
                }
            }

            return false;
        }

        static bool cookie_verify(const httplib::Request& req,httplib::Response& res)
        {
            std::string cookie = req.get_header_value("Cookie");
            if (cookie.empty() && req.path != "/login.html")
            {
                res.set_header("Location", "http://123.60.163.45:8888/login.html");
                res.status = 302;  // 设置 HTTP 状态码为 302，表示重定向
                //std::cout << "没有 Cookie,重定向到登录页面...\n";
                return true;
            }

            return false;
        }
    };
    


}