add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_library(memorylib "${CMAKE_CURRENT_SOURCE_DIR}/memory.c" "${CMAKE_CURRENT_SOURCE_DIR}/memory.h")
