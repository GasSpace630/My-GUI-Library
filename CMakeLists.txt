cmake_minimum_required(VERSION 3.10)
project(my_guilib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(my_guilib
    src/main.cpp
)

target_link_libraries(my_guilib raylib)
