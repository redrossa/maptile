cmake_minimum_required(VERSION 3.17)
project(maptile C)

set(CMAKE_C_STANDARD 11)

add_executable(maptile main.c map.h map.c)