cmake_minimum_required(VERSION 3.18)
project(Zachranka C)

set(CMAKE_C_STANDARD 99)

add_executable(Zachranka main.c)

target_link_libraries(Zachranka pthread m)