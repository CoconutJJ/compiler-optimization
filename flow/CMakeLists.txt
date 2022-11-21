add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_library(flowlib "${CMAKE_CURRENT_SOURCE_DIR}/flow.c" "${CMAKE_CURRENT_SOURCE_DIR}/flow.h")
target_include_directories(flowlib PUBLIC "${CMAKE_SOURCE_DIR}/definition")
target_include_directories(flowlib PUBLIC "${CMAKE_SOURCE_DIR}/variable")
target_include_directories(flowlib PUBLIC "${CMAKE_SOURCE_DIR}/integer_set")
target_include_directories(flowlib PUBLIC "${CMAKE_SOURCE_DIR}/basicblock")
target_include_directories(flowlib PUBLIC "${CMAKE_SOURCE_DIR}/memory")

