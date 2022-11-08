add_executable(test_integer_set_1 "${CMAKE_CURRENT_SOURCE_DIR}/test_integer_set_1.c")
add_test(IntegerSet_test_can_store_integer test_integer_set_1)
target_include_directories(test_integer_set_1 PUBLIC "${CMAKE_SOURCE_DIR}/integer_set")
target_link_libraries(test_integer_set_1 PUBLIC intsetlib)

add_executable(test_integer_set_2 "${CMAKE_CURRENT_SOURCE_DIR}/test_integer_set_2.c")
add_test(IntegerSet_test_can_iterate_integer test_integer_set_2)
target_include_directories(test_integer_set_2 PUBLIC "${CMAKE_SOURCE_DIR}/integer_set")
target_link_libraries(test_integer_set_2 PUBLIC intsetlib)

add_executable(test_integer_set_3 "${CMAKE_CURRENT_SOURCE_DIR}/test_integer_set_3.c")
add_test(IntegerSet_test_can_subtract test_integer_set_3)
target_include_directories(test_integer_set_3 PUBLIC "${CMAKE_SOURCE_DIR}/integer_set")
target_link_libraries(test_integer_set_3 PUBLIC intsetlib)