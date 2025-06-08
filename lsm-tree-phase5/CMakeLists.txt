cmake_minimum_required(VERSION 3.10)
project(LSM_TREE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(embedding)

add_executable(report report.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(Vector_P1 Vector_Persistent_Test_Phase1.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(Vector_P2 Vector_Persistent_Test_Phase2.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(HNSW_Delete HNSW_Delete_Test.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(HNSW_P1 HNSW_Persistent_Test_Phase1.cpp kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(HNSW_P2 HNSW_Persistent_Test_Phase2.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(HNSW_P3 HNSW_Persistent_Test_Phase1_complex.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

add_executable(HNSW_P4 HNSW_Persistent_Test_Phase2_complex.cpp kvstore_api.h kvstore.h kvstore.cc
        skiplist.cpp skiplist.h sstable.cpp sstable.h HNSW.h HNSW.cc emtable.h emtable.cpp
        bloom.cpp bloom.h MurmurHash3.h utils.h test.h readEmbedding.h threadpool.h
        sstablehead.cpp sstablehead.h)

#set_target_properties(E2E_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

add_subdirectory(third_party/llama.cpp)

add_subdirectory(embedding)

add_subdirectory(test)

target_link_libraries(report PRIVATE embedding)
target_link_libraries(Vector_P1 PRIVATE embedding)
target_link_libraries(Vector_P2 PRIVATE embedding)
target_link_libraries(HNSW_Delete PRIVATE embedding)
target_link_libraries(HNSW_P1 PRIVATE embedding)
target_link_libraries(HNSW_P2 PRIVATE embedding)
target_link_libraries(HNSW_P3 PRIVATE embedding)
target_link_libraries(HNSW_P4 PRIVATE embedding)