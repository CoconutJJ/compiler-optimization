cmake_minimum_required(VERSION 3.24)
project(compiler C)

set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -Wconversion -Wsign-compare")
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

list(APPEND Directories variable definition block value integer_set flow memory)

foreach(dir IN LISTS Directories)
    add_subdirectory(${dir})
endforeach()

enable_testing()
add_subdirectory(tests)

add_executable(compiler main.c)
target_include_directories(compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/variable")
target_include_directories(compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/definition")

foreach(dir IN LISTS Directories)
    target_link_libraries(compiler PUBLIC "${dir}lib")
endforeach()