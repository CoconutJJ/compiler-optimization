add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(intsetlib "${CMAKE_CURRENT_SOURCE_DIR}/integer_set.c" "${CMAKE_CURRENT_SOURCE_DIR}/integer_set.h")
