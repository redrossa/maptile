cmake_minimum_required(VERSION 3.17)
project(maptile)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC src/*)
file(GLOB INCLUDE include/*)

add_executable(maptile main.cpp ${SRC} ${INCLUDE})

find_package(CURL CONFIG REQUIRED)
target_link_libraries(maptile PRIVATE CURL::libcurl)

find_package(ZLIB REQUIRED)
target_link_libraries(maptile PRIVATE ZLIB::ZLIB)