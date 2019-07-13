project(201907TLTEST)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CODE_BASE ${CMAKE_CURRENT_BINARY_DIR}/../code)
set(GTEST_BASE ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/gtest-1.7.0)
set(JSON_BASE ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/cJSON)

#add_definitions("-std=c++11")

add_subdirectory(code)
add_subdirectory(3rdparty/gtest-1.7.0)
add_subdirectory(3rdparty/cJSON)
add_subdirectory(test)
add_subdirectory(exe)
