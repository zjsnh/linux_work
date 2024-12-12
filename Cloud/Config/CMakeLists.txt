cmake_minimum_required(VERSION 3.10)

project(Cloud)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 包含目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加库目录
link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(/usr/lib64/mysql)
link_directories(${PROJECT_SOURCE_DIR}/Lib)

# 添加可执行文件
add_executable(text code.cpp)

# 链接库
target_link_libraries(text
    mysqlclient
    stdc++fs
    pthread
    bundle
    jsoncpp
)
