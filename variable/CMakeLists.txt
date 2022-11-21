add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_library(variablelib "${CMAKE_CURRENT_SOURCE_DIR}/variable.c" "${CMAKE_CURRENT_SOURCE_DIR}/variable.h")
target_include_directories(variablelib PUBLIC "${CMAKE_SOURCE_DIR}/variable")
target_include_directories(variablelib PUBLIC "${CMAKE_SOURCE_DIR}/mem")

