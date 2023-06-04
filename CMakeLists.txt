cmake_minimum_required(VERSION 3.25)
project(tinyconfig C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(tinyconfig tinyconfig.c)
