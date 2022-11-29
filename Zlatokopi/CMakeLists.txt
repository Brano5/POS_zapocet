cmake_minimum_required(VERSION 3.18)
project(Zlatokopi C)

set(CMAKE_C_STANDARD 99)

add_executable(Zlatokopi main.c)

target_link_libraries(Zlatokopi pthread m)