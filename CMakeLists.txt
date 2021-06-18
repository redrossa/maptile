cmake_minimum_required(VERSION 3.17)
project(maptile C)

set(CMAKE_C_STANDARD 11)

file(GLOB SRC src/*.c)
file(GLOB INCLUDE include/*.h)

add_executable(maptile main.c ${SRC} ${INCLUDE})

add_executable(linear_download examples/linear_download.c ${SRC} ${INCLUDE})
add_executable(multi_download examples/multi_download.c ${SRC} ${INCLUDE})
add_executable(merge examples/merge.c ${SRC} ${INCLUDE})
add_executable(spng_test examples/spng_test.c ${SRC} ${INCLUDE})

find_package(CURL CONFIG REQUIRED)
target_link_libraries(maptile PRIVATE CURL::libcurl)
target_link_libraries(linear_download PRIVATE CURL::libcurl)
target_link_libraries(multi_download PRIVATE CURL::libcurl)
target_link_libraries(merge PRIVATE CURL::libcurl)
target_link_libraries(spng_test PRIVATE CURL::libcurl)
