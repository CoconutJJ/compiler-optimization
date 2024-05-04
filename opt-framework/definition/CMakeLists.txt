add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(definitionlib "${CMAKE_CURRENT_SOURCE_DIR}/definition.c" "${CMAKE_CURRENT_SOURCE_DIR}/definition.h")
target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/variable")
target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/mem")
target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/expression")
target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/linked_list")
