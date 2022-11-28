cmake_minimum_required(VERSION 3.18)
project(Tele C)

set(CMAKE_C_STANDARD 99)

add_executable(Tele main.c)

target_link_libraries(Tele pthread m)