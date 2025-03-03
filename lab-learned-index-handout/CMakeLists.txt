cmake_minimum_required(VERSION 3.10)
project(learned-index)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(test)

target_include_directories(learned_index PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
