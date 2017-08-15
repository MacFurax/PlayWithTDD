cmake_minimum_required( VERSION 3.5)
project(playWithTDD)


include_directories(/usr/local/include .)
link_directories(/usr/local/lib)

find_package (Threads)


set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable( busTest  ./AUTOKAR/busTest.cpp) 
add_executable( transportTest  ./AUTOKAR/transportTest.cpp)

target_link_libraries( 
    busTest 
    
    ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries( 
    transportTest
    
    PocoFoundation
    PocoNet
)