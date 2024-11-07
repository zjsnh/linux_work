#include "config.hpp"
#include "data.hpp"

void data_hpp()
{
    // Cloud::BackupInfo info;
    // info.NewBackupInfo("bundle.h");

    Cloud::DataManager data;
    // data.Insert(info);
    // info.NewBackupInfo("util.hpp");
    // data.Insert(info);

    std::vector<Cloud::BackupInfo> array;
    data.GetAll(&array);

    for(const auto& e:array)
    {
        std::cout << e.url_path << std::endl;
    }

    return;
}

int main()
{
    //data_hpp();

    return 0;
}