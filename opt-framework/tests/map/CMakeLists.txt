
list(APPEND TestFiles "test_map_1")

list(APPEND TestNames 
    "Map_test_can_create_map" 
)

foreach(testfile testname IN ZIP_LISTS TestFiles TestNames)
    add_executable(${testfile} "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}.c")
    add_test(${testname} ${testfile})
    target_include_directories(${testfile} PUBLIC "${CMAKE_SOURCE_DIR}/map")
    target_link_libraries(${testfile} PUBLIC maplib)
endforeach()
