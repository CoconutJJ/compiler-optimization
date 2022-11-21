
list(APPEND TestFiles 
    test_memory_1 
)

list(APPEND TestNames 
    memory_test_can_allocate_memory 
)

foreach(testfile testname IN ZIP_LISTS TestFiles TestNames)
    add_executable(${testfile} "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}.c")
    add_test(${testname} ${testfile})
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/memory")
    target_link_libraries(${testfile} PUBLIC variablelib)
    target_link_libraries(${testfile} PUBLIC memorylib)
endforeach()
