cmake_minimum_required(VERSION 3.18)
project(KavaPreDoktoranda C)

set(CMAKE_C_STANDARD 99)

add_executable(KavaPreDoktoranda main.c)

target_link_libraries(KavaPreDoktoranda pthread m)
