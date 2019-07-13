project(gtest)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(SRC_FILES 
	"${CMAKE_CURRENT_LIST_DIR}/src/gtest-all.cc"
	"${CMAKE_CURRENT_LIST_DIR}/src/gtest_main.cc"
	)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR})

add_library(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
