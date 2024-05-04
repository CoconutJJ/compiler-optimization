add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_library(memlib "${CMAKE_CURRENT_SOURCE_DIR}/mem.c" "${CMAKE_CURRENT_SOURCE_DIR}/mem.h")
