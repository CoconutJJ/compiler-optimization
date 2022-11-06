add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(blocklib "${CMAKE_CURRENT_SOURCE_DIR}/block.c" "${CMAKE_CURRENT_SOURCE_DIR}/block.h")
target_include_directories(blocklib PUBLIC "${CMAKE_SOURCE_DIR}/block")
target_include_directories(blocklib PUBLIC "${CMAKE_SOURCE_DIR}/definition")
target_include_directories(blocklib PUBLIC "${CMAKE_SOURCE_DIR}/variable")

