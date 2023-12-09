add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_library(maplib "${CMAKE_CURRENT_SOURCE_DIR}/map.c" "${CMAKE_CURRENT_SOURCE_DIR}/map.h")
