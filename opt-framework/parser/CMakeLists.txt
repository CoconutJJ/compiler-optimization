add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(parserlib "${CMAKE_CURRENT_SOURCE_DIR}/token.c" "${CMAKE_CURRENT_SOURCE_DIR}/parser.h")
