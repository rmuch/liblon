cmake_minimum_required(VERSION 2.8)
project(lon)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --analyze")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)
