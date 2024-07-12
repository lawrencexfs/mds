cmake_minimum_required(VERSION 3.10)
project(bidemo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加一个可执行文件
add_executable(demo bigint.cpp)

# 链接库
target_link_libraries(demo PRIVATE ssl uuid boost_system)

# cmake .. -DCMAKE_CXX_FLAGS="-fcoroutines -O2"
