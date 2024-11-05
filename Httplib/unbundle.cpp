#include <iostream>
#include <fstream>

#include "bundle.h"




int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        std::cout << "/r/n" << argv[0] << "+ file1.lz  file2" << std::endl;
    }

    std::ifstream ifs;
    ifs.open(argv[1], std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string content(size, '\0');
    ifs.read(&content[0], size);
    ifs.close();

    std::string unpack = bundle::unpack(content);
    std::ofstream ofs;
    std::string filename = argv[2];
    ofs.open(argv[2],std::ios::binary);
    ofs.write(unpack.c_str(), unpack.size());
    ofs.close();

    return 0;
}