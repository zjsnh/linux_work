#include <iostream>
#include <mysql/mysql.h>
//#include "data_text.hpp"


bool Insert(MYSQL* mysql)
{
    //对于 
    /*const char *insert_or_update_query = "INSERT INTO file_info (file_id, file_size, created_time, last_access_time, modified_time, compressed_status, file_hash) "
                                     "VALUES ('file124', 102400, '2024-11-19 12:00:00', '2024-11-19 13:00:00', '2024-11-19 14:00:00', 1, 'abc123hashvalue') "
                                     "ON DUPLICATE KEY UPDATE file_size = 102400, modified_time = '2024-11-19 14:00:00', compressed_status = 1, file_hash = 'abc123hashvalue'";

    if (mysql_query(conn, insert_or_update_query)) {
        std::cerr << "INSERT/UPDATE failed: " << mysql_error(conn) << "\n";
    } else {
        std::cout << "Record inserted or updated successfully\n";
    }*/


    std::string filename = "file124";
    std::string file_size = "102460";
    std::string last_access_time = "11188888811111";
    std::string modified_time = "222222233333333";  // 修正时间格式
    int compressed_status = 1;  // 假设1表示压缩状态
    std::string real_path = "./real_path_/" + filename;
    std::string pack_path = "./pack_path_/" + filename + ".lz";
    std::string url_path = "./download_/" + filename;

    std::string insert_query = "INSERT INTO file_info (filename, fsize, Atime, Mtime, pack_flag, real_path, pack_path, url_path) "
                            "VALUES ('" + filename + "', " 
                            + file_size + ", '" 
                            + last_access_time + "', '" 
                            + modified_time + "', " 
                            + std::to_string(compressed_status) + ", '" 
                            + real_path + "', '" 
                            + pack_path + "', '" 
                            + url_path + "') "
                            "ON DUPLICATE KEY UPDATE "
                            "fsize = VALUES(fsize), "
                            "Atime = VALUES(Atime), "
                            "Mtime = VALUES(Mtime), "
                            "pack_flag = VALUES(pack_flag), "
                            "real_path = VALUES(real_path), "
                            "pack_path = VALUES(pack_path), "
                            "url_path = VALUES(url_path)";


    if(mysql_query(mysql, insert_query.c_str()))
    {
        std::cerr << "Result storing failed: " << mysql_error(mysql) << "\n";
    }


    return true;
}

bool Select(MYSQL* mysql)
{
    //查询全部数据，进行格式输出
    std::string select_query = "select *from file_info";

    mysql_query(mysql, select_query.c_str());
    MYSQL_RES *res = mysql_store_result(mysql);
    if(res)
    {
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res)))
        {
            // 假设表有7列，file_id, file_size, created_time, last_access_time, modified_time, compressed_status, file_hash
            std::cout << "file_id: " << row[0] << ", file_size: " << row[1] << ", created_time: " << row[2]
                    << ", last_access_time: " << row[3] << ", modified_time: " << row[4]
                    << ", compressed_status: " << row[5] << ", file_hash: " << row[6] << ", file_hash: " << row[7]<< "\n";
        }
        mysql_free_result(res);
    }
    else{
        std::cerr << "Result storing failed: " << mysql_error(mysql) << "\n";
    }

    return true;
}

bool Update(MYSQL* mysql)
{

}

bool Delete(MYSQL* mysql)
{

}
int main()
{
    //std::cout << "clinet version " << mysql_get_client_info();
    MYSQL *mysql;
    MYSQL_RES *res;
    MYSQL_ROW row;

    mysql = mysql_init(NULL);

    if(mysql == NULL)
    {
        std::cout << "mysql initialization failed \n";
        return 1;
    }

    mysql = mysql_real_connect(mysql, "localhost", "zws", "123456", "Cloud", 3306, NULL, 0);
    if (mysql == NULL)
    {
        std::cout << "mysql connect failed \n";
        return 2;
    }

    Insert(mysql);
    Select(mysql);

    return 0;
}
