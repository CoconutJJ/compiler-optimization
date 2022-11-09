
list(APPEND TestFiles 
    test_integer_set_1 
    test_integer_set_2 
    test_integer_set_3 
    test_integer_set_4 
    test_integer_set_5 
    test_integer_set_6 

)

list(APPEND TestNames 
    IntegerSet_test_can_store_integer 
    IntegerSet_test_can_iterate_integer 
    IntegerSet_test_can_subtract 
    IntegerSet_test_can_intersect 
    IntegerSet_test_can_union
    IntegerSet_test_can_union_universal_set

)

foreach(testfile testname IN ZIP_LISTS TestFiles TestNames)
    add_executable(${testfile} "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}.c")
    add_test(${testname} ${testfile})
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/integer_set")
    target_link_libraries(${testfile} PUBLIC integer_setlib)
endforeach()
