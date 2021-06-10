cmake_minimum_required(VERSION 3.17)
project(maptile C)

set(CMAKE_C_STANDARD 11)

add_executable(maptile main.c map.h map.c)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(maptile ${CURL_LIBRARIES})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(maptile ${PNG_LIBRARIES})