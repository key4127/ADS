cmake_minimum_required(VERSION 3.10)
project(LSM_TREE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(embedding)

#[[add_executable(correctness correctness.cc kvstore_api.h kvstore.h
        kvstore.cc skiplist.cpp skiplist.h sstable.cpp sstable.h
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)]]

#[[add_executable(persistence persistence.cc kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)]]

add_executable(E2E_test test/E2E_test.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)

add_executable(report report.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)

set_target_properties(E2E_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

add_subdirectory(third_party/llama.cpp)

add_subdirectory(embedding)

add_subdirectory(test)

# target_link_libraries(correctness PRIVATE embedding)
# target_link_libraries(persistence PRIVATE embedding)
target_link_libraries(E2E_test PRIVATE embedding)
target_link_libraries(report PRIVATE embedding)