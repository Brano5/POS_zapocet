cmake_minimum_required(VERSION 3.18)
project(kombi C)

set(CMAKE_C_STANDARD 99)

add_executable(kombi main.c)

target_link_libraries(kombi pthread m)
