cmake_minimum_required (VERSION 3.8)

project("ThreadPool")

set(CMAKE_CXX_STANDARD 17) # TODO: check compatibility
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread") # TODO: cross-platform way

add_library(tpool src/worker.cpp)

if(MSVC)
  target_compile_options(tpool PUBLIC /W4 /WX)
else()
  target_compile_options(tpool PUBLIC -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(tpool PUBLIC inc/)

add_subdirectory("demo")