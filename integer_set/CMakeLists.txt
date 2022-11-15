add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(integer_setlib "${CMAKE_CURRENT_SOURCE_DIR}/integer_set.c" "${CMAKE_CURRENT_SOURCE_DIR}/integer_set.h")
target_include_directories(integer_setlib PUBLIC "${CMAKE_SOURCE_DIR}/memory")