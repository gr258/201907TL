project(EXECUTE_PROGRAM)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/src/*.*
    ${CMAKE_CURRENT_LIST_DIR}/include/*.*
)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CODE_BASE}/include
)


add_executable(${PROJECT_NAME} ${SRC_FILES})

add_dependencies(${PROJECT_NAME} SOURCE_CODE)

target_link_libraries(${PROJECT_NAME} SOURCE_CODE json)
