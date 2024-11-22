#pragma once

#include "data.hpp"
#include "httplib.h"
#include "config.hpp" 
#include "usr.hpp"
extern Cloud::DataManager *data;

namespace Cloud
{
    static std::string wwwroot = "wwwroot";
    static std::string err = "wwwroot/err.html";
    static std::string Home = "index.html";

   

    class Server
    {
    private:
        int port_;
        std::string ip_;
        std::string url_prefix_;
        std::string download_prefix_;
        httplib::Server svr_;
        
         // 根据文件扩展名获取 MIME 类型
        static std::string GetMimeType(const std::string &path) {
            static const std::unordered_map<std::string, std::string> mime_types = {
                {".html", "text/html"},
                {".htm", "text/html"},
                {".png", "image/png"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".gif", "image/gif"},
                {".css", "text/css"},
                {".js", "application/javascript"}
            };

            auto ext_pos = path.find_last_of('.');
            if (ext_pos != std::string::npos) {
                std::string ext = path.substr(ext_pos);
                auto it = mime_types.find(ext);
                if (it != mime_types.end()) {
                    return it->second;
                }
            }
            return "application/octet-stream"; // 默认 MIME 类型
        }

    public:

        Server()
        {
            Config *con = Config::GetInstance();
            port_ = con->GetServerPort();
            ip_ = con->GetServerIp();

            url_prefix_ = con->GetURLPrefix();
        }

        static void Err(httplib::Response& res)
        {
            FileUtil up(err);
            std::string body;
            up.GetContent(&body);
            res.status = 404;

            res.set_content(body, "text/html");
            return;
        }

        static std::string UserName(const httplib::Request& req) 
        {
            std::unordered_map<std::string, std::string> cookies;
            std::string cookie = req.get_header_value("Cookie");

            std::cout << cookie << std::endl;

            size_t pos = 0, idx = 0;
            std::string sep = ";";
            while (true)
            {
                pos = cookie.find(";", idx);
                if (pos == std::string::npos)
                    break;

                std::string _str = cookie.substr(idx, pos);
                size_t pos_ = _str.find("=");
                cookies[_str.substr(0, pos_)] = _str.substr(pos_ + 1);

                cookie = cookie.substr(pos + 2);
            }
            if (cookie.empty() == false)
            {
                size_t pos_ = cookie.find("=");
                cookies[cookie.substr(0, pos_)] = cookie.substr(pos_ + 1);
            }

            return cookies["username"];
        }

        static void Upload(const httplib::Request& req,httplib::Response& res)
        {
            if(Usr::cookie_verify(req,res))
                return;

            
            auto ret = req.has_file("file");
            if(ret = false)
            {
                Err(res);
            }

            const auto &file = req.get_file_value("file");
            std::string back_dir = Config::GetInstance()->GetBackDir();
            std::string realpath = back_dir + FileUtil(file.filename).FileName();

            FileUtil fu(realpath);
            fu.SetContent(file.content);

            BackupInfo info;
            info.NewBackupInfo(realpath);
            data->Insert(info,UserName(req));

            std::cout << "Upload success!" << fu.FileName() << std::endl;

            return;
        }

        static void ListShow(const httplib::Request& req,httplib::Response& res)
        {
            // if(Usr::cookie_verify(req,res))
            //     return;
            
            std::vector<BackupInfo> arry;
            data->GetAll(&arry,UserName(req));

            //2. 根据所有备份信息，组织html文件数据
            std::stringstream ss;
            ss << "<html><head><title>Download</title><meta charset=\"UTF-8\"></head>";

            ss << "<body><h1>Download</h1><table>";
            

            for (auto &a : arry){
					ss << "<tr>";
					std::string filename = FileUtil(a.real_path).FileName();
					ss << "<td><a href='" << a.url_path << "'>" << filename << "</a></td>";
					ss << "<td align='right'>" << FileUtil::FormatTime(a.Atime) << "</td>";
					ss << "<td align='right'>" << a.fsize / 1024 << "k</td>";
					ss << "</tr>";
				}
            ss << "</table></body></html>";
            res.body = ss.str();
            res.set_header("Content-Type", "text/html");
            res.status = 200;
            return ;

        }

        static void index(const httplib::Request& req,httplib::Response& res)
        {
            if(Usr::cookie_verify(req,res))
                return;
            std::string file_path = wwwroot + req.path;

            if (file_path == wwwroot + "/") {
                file_path += Home;
            }

             //  "/upload"  "/index"

            std::string body;

            FileUtil up(file_path);
            if(up.Exists() == false)
            {
                Err(res);
            }

            //std::cout << "file exist\n";

            up.GetContent(&body);

            std::string mime_type = GetMimeType(file_path);
            res.set_content(body, mime_type);
        }

        static std::string GetETag(const BackupInfo& info)
        {
            FileUtil fu(info.real_path);
            std::string etag = fu.FileName();
            etag += "-";
            etag += std::to_string(info.fsize);
            etag += "-";
            etag += std::to_string(info.Mtime);

            return etag;
        }

        static void Download(const httplib::Request& req,httplib::Response& res)
        {
            if(Usr::cookie_verify(req,res))
                return;

            BackupInfo info;
            data->GetOneByURL(req.path, &info);

            //std::cout << info.real_path << std::endl;

            if(info.pack_flag == true)
            {
                //解压删除
                FileUtil fu(info.pack_path);
                //std::cout << info.real_path << " ::" << std::endl;

                fu.uncompare(info.real_path);

                fu.Remove();
                info.pack_flag = false;
                data->Updata(info);
            }

            bool retrans = false;
            std::string old_etag;
            if (req.has_header("If-Range"))
            {
                old_etag = req.get_header_value("If-Range");
                if(old_etag == GetETag(info))
                {
                    retrans = true;
                }
            }

            FileUtil fu(info.real_path);

            if(retrans == false)
            {
                fu.GetContent(&res.body);
                res.set_header("Accept-Ranges", "bytes");
                res.set_header("ETag", GetETag(info));
                res.set_header("Content-Type", "application/octet-stream");

                res.status = 200;
            }
            else{

                fu.GetContent(&res.body);
                res.set_header("Accept-Ranges", "bytes");
                res.set_header("ETag", GetETag(info));
                res.set_header("Content-Type", "application/octet-stream");

                res.status = 206;

            }

            return;
        }

        static void Login(const httplib::Request& req,httplib::Response& res)
        {

            std::string username = req.get_param_value("username");
            std::string password = req.get_param_value("password");

            if(Usr::verify(username,password))
            {
                // 设置 Cookie
                std::string cookie_username = "username=" + username;
                std::string cookie_password = " password=" + password;

                res.set_header("Set-Cookie", cookie_password);
                res.set_header("Set-Cookie", cookie_username);
               

                // 返回成功响应
                res.set_content("success", "text/plain");
                return;
            }

            res.set_content("false", "text/plain");
            return;
        }

        void RunModule()
        {
            svr_.Post("/Upload", Upload);
            svr_.Post("/Login", Login);

            svr_.Get("/", index);
            svr_.Get("/index.html", index);
            svr_.Get("/listshow", ListShow);
            svr_.Get("/upload.html", index);
            svr_.Get("/login.html", index);

            std::string download_url = url_prefix_ + "(.*)";
            svr_.Get(download_url, Download);

            // std::cout << ip_ << std::endl;
            // std::cout << port_ << std::endl;
            std::cout << svr_.listen("0.0.0.0", port_, 0);
        }
    };
}