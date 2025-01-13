cmake_minimum_required(VERSION 3.10)
project(taskplug)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加编译选项，包括 -pthread
add_compile_options(-pthread)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=auto")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/head)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCES)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

# target_link_libraries(${PROJECT_NAME} PRIVATE boost_system lz4 z)
