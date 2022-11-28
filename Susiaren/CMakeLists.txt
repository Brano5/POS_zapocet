cmake_minimum_required(VERSION 3.18)
project(Susiaren C)

set(CMAKE_C_STANDARD 99)

add_executable(Susiaren main.c)

target_link_libraries(Susiaren pthread m)