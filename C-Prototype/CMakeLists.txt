cmake_minimum_required(VERSION 3.22)
project(C_Prototype C)

set(CMAKE_C_STANDARD 99)

add_executable(C_Prototype main.c)
target_link_libraries(C_Prototype ws2_32)



