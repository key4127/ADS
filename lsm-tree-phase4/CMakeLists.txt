cmake_minimum_required(VERSION 3.10)
project(LSM_TREE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(embedding)

#[[add_executable(E2E_test test/E2E_test.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)]]

#[[add_executable(report report.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)]]

add_executable(Vector_Persistent_Test_Phase1 Vector_Persistent_Test_Phase1.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)

add_executable(Vector_Persistent_Test_Phase2 Vector_Persistent_Test_Phase2.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)

add_executable(HNSW_Delete_Test HNSW_Delete_Test.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h
        sstablehead.cpp sstablehead.h)

#set_target_properties(E2E_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

add_subdirectory(third_party/llama.cpp)

add_subdirectory(embedding)

add_subdirectory(test)

#target_link_libraries(E2E_test PRIVATE embedding)
#target_link_libraries(report PRIVATE embedding)
target_link_libraries(Vector_Persistent_Test_Phase1 PRIVATE embedding)
target_link_libraries(Vector_Persistent_Test_Phase2 PRIVATE embedding)
target_link_libraries(HNSW_Delete_Test PRIVATE embedding)