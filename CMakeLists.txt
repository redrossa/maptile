cmake_minimum_required(VERSION 3.17)
project(maptile)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC src/*.cpp src/*.c)
file(GLOB INCLUDE include/*.h)

add_executable(maptile main.cpp ${SRC} ${INCLUDE})

find_package(CURL CONFIG REQUIRED)
target_link_libraries(maptile PRIVATE CURL::libcurl)