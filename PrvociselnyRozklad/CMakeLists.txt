cmake_minimum_required(VERSION 3.18)
project(PrvociselnyRozklad C)

set(CMAKE_C_STANDARD 99)

add_executable(PrvociselnyRozklad main.c)

target_link_libraries(PrvociselnyRozklad pthread)
