cmake_minimum_required( VERSION 3.5)
project(playWithTDD)

find_package (Threads)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable( busTest  ./AUTOKAR/busTest.cpp) 
add_executable( transportTest  ./AUTOKAR/transportTest.cpp)

target_link_libraries ( busTest ${CMAKE_THREAD_LIBS_INIT})