cmake_minimum_required(VERSION 3.18)
project(Bar C)

set(CMAKE_C_STANDARD 99)

add_executable(Bar main.c)

target_link_libraries(Bar pthread m)
