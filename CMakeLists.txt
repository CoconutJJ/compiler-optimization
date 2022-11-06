cmake_minimum_required(VERSION 3.24)
project(compiler C)

add_subdirectory(variable)
add_subdirectory(definition)
add_subdirectory(block)
add_subdirectory(value)
add_subdirectory(integer_set)

add_executable(compiler main.c)

target_include_directories(compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/variable")
target_include_directories(compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/definition")
target_link_libraries(compiler PUBLIC variablelib)

target_link_libraries(compiler PUBLIC definitionlib)
target_link_libraries(compiler PUBLIC blocklib)
target_link_libraries(compiler PUBLIC valuelib)
target_link_libraries(compiler PUBLIC intsetlib)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)