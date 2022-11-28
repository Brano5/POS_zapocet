cmake_minimum_required(VERSION 3.18)
project(ZberCaju C)

set(CMAKE_C_STANDARD 99)

add_executable(ZberCaju main.c)

target_link_libraries(ZberCaju pthread m)