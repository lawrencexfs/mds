cmake_minimum_required(VERSION 3.10)
project(bidemo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckIncludeFileCXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=auto")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SumHead)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} bigint.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE boost_system)
