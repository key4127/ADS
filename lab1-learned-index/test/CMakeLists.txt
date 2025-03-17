add_executable(basic_test1 basic_test1.cc)

target_link_libraries(basic_test1 learned_index)

add_executable(basic_test2 basic_test2.cc)

target_link_libraries(basic_test2 learned_index)

add_executable(large_test1 large_test1.cc)

target_link_libraries(large_test1 learned_index)

add_executable(large_test2 large_test2.cc)

target_link_libraries(large_test2 learned_index)

add_executable(compare_test compare_test.cc)

target_link_libraries(compare_test learned_index)
