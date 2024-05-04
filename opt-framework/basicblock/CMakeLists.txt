add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(basicblocklib "${CMAKE_CURRENT_SOURCE_DIR}/basicblock.c" "${CMAKE_CURRENT_SOURCE_DIR}/basicblock.h")
target_include_directories(basicblocklib PUBLIC "${CMAKE_SOURCE_DIR}/definition")
target_include_directories(basicblocklib PUBLIC "${CMAKE_SOURCE_DIR}/variable")
target_include_directories(basicblocklib PUBLIC "${CMAKE_SOURCE_DIR}/integer_set")
target_include_directories(basicblocklib PUBLIC "${CMAKE_SOURCE_DIR}/mem")

target_include_directories(basicblocklib PUBLIC "${CMAKE_SOURCE_DIR}/linked_list")
target_include_directories(basicblocklib PUBLIC "${CMAKE_SOURCE_DIR}/expression")
