#include <iostream>
#include <regex>
#include <string>


//  ^?  匹配非 ? 字符

void match()
{
    std::string url = "GET /bitejuiyeke/login?user=xiaoming&pass=123123 HTTP/1.1";
    std::smatch matches;

    std::regex e("(GET|POST|HEAD|PUT|DELETE) ([^?]*)(?:\\?(.*))? (HTTP/1\\.[01])(?:\n|\r\n)?");

    bool ret = std::regex_match(url, matches, e);

    if(ret == false)
        return;

    for(const auto& e:matches)
    {
        std::cout << e << std::endl;
    }

    return ;
}

int main()
{
    match();
    return 0;
}