cmake_minimum_required(VERSION 3.18)
project(SialeneOdmeny C)

set(CMAKE_C_STANDARD 99)

add_executable(SialeneOdmeny main.c)

target_link_libraries(SialeneOdmeny pthread m)