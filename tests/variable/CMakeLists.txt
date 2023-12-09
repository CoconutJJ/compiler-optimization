
list(APPEND TestFiles "test_variable_1")

list(APPEND TestNames 
    "Variable_test_can_create_variable" 
)

foreach(testfile testname IN ZIP_LISTS TestFiles TestNames)
    add_executable(${testfile} "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}.c")
    add_test(${testname} ${testfile})
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/variable")
    target_link_libraries(${testfile} PUBLIC variablelib)
    target_link_libraries(${testfile} PUBLIC memlib)
endforeach()
