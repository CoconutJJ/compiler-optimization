add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(linked_listlib "${CMAKE_CURRENT_SOURCE_DIR}/linked_list.c" "${CMAKE_CURRENT_SOURCE_DIR}/linked_list.h")

