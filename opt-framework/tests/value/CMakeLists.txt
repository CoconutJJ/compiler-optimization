
list(APPEND TestFiles "test_value_numbering_1")

list(APPEND TestNames 
    "Value_test_can_create_value_numbering_table" 
)

foreach(testfile testname IN ZIP_LISTS TestFiles TestNames)
    add_executable(${testfile} "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}.c")
    add_test(${testname} ${testfile})
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/value")
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/variable")
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/definition")
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/expression")

    target_link_libraries(${testfile} PUBLIC valuelib)
    target_link_libraries(${testfile} PUBLIC variablelib)
    target_link_libraries(${testfile} PUBLIC definitionlib)
    target_link_libraries(${testfile} PUBLIC expressionlib)
endforeach()
