cmake_minimum_required (VERSION 3.8)

project("ThreadPool")

set(CMAKE_CXX_STANDARD 17) # TODO: check compatibility

add_library(tpool INTERFACE)

target_include_directories(tpool INTERFACE inc/)

add_subdirectory("demo")