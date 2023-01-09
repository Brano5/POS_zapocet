cmake_minimum_required(VERSION 3.18)
project(DadaZap C)

set(CMAKE_C_STANDARD 99)

add_executable(DadaZap main.c)

target_link_libraries(DadaZap pthread m)