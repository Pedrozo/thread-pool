cmake_minimum_required (VERSION 3.8)

project("ThreadPool")

set(CMAKE_CXX_STANDARD 17) # TODO: check compatibility
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

add_library(tpool src/instruction_queue.cpp src/worker.cpp)

target_include_directories(tpool PUBLIC inc/)

add_subdirectory("demo")