#include <iostream>
#include <fstream>
#include <unordered_map>

#include "bundle.h"
#include "httplib.h"


// 静态变量定义
static std::string wwwroot = "wwwroot";
static std::string err = "wwwroot/err.html";
static std::string Home = "index.html";

static std::string zip = ".lz";

// 根据文件扩展名获取 MIME 类型
std::string GetMimeType(const std::string &path) {
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

// 通用文件读取函数
std::string ReadFileContent(const std::string &file_path) {
    std::ifstream in(file_path, std::ios::binary); // 二进制模式打开
    if (!in.is_open()) {
        return ""; // 文件打开失败，返回空字符串表示错误
    }

    in.seekg(0, std::ios::end);
    auto len = in.tellg();
    in.seekg(0, std::ios::beg);

    std::string content(len, '\0');
    in.read(&content[0], len);
    in.close();

    return content;
}

void Bundle(const std::string& file_path)
{
    std::string body = ReadFileContent(file_path);
    std::string packed = bundle::pack(bundle::LZIP, body);

    std::ofstream ofs;
    std::string ofilename = file_path + zip;
    ofs.open(ofilename, std::ios::binary);
    ofs.write(packed.c_str(), packed.size());

    ofs.close();
}

void Multipart(const httplib::Request &req, httplib::Response &res)
{
    if(req.has_file("file"))
    {
        const auto &file = req.get_file_value("file");

        std::cout << "Rec file:" << file.filename << "(" << file.content.size() << "bytes )" << std::endl;

        std::string file_path = "download/" + file.filename;
        std::ofstream ofs(file_path, std::ios::binary);
        if(ofs)
        {
            ofs.write(file.content.data(), file.content.size());
            ofs.close();

            //对文件进行压缩
            Bundle(file_path);

            res.status = 200;
            res.set_content("file uploaded successfully....", "text/plain");

        } else {
            res.status = 500;  // 内部服务器错误
            res.set_content("Failed to save the file.", "text/plain");
        }
    } 
    else {
        res.status = 400;  // 请求错误
        res.set_content("No file uploaded.", "text/plain");
    }
}

int main() {
    httplib::Server svr;

    //处理通用文件请求
    svr.Get(R"(^(/[^/\0]*)*$)", [](const httplib::Request &req, httplib::Response &res) {
        std::string file_path = wwwroot + req.path;
        
        if (file_path == wwwroot + "/") {
            file_path += Home;
        }

        std::string content = ReadFileContent(file_path);
        if (content.empty()) {
            content = ReadFileContent(err);
            res.status = 404;
        }

        std::string mime_type = GetMimeType(file_path);
        res.set_content(content, mime_type);
    });

    svr.Post("/multipart", Multipart);

    std::cout << "Server is running on http://localhost:8888" << std::endl;
    svr.listen("0.0.0.0", 8888);

    return 0;
}
