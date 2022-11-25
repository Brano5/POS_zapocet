cmake_minimum_required(VERSION 3.18)
project(2Drinkstone C)

set(CMAKE_C_STANDARD 99)

add_executable(2Drinkstone main.c)

target_link_libraries(2Drinkstone pthread m)
