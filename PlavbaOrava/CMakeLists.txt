cmake_minimum_required(VERSION 3.18)
project(PlavbaOrava C)

set(CMAKE_C_STANDARD 99)

add_executable(PlavbaOrava main.c)

target_link_libraries(PlavbaOrava pthread m)
