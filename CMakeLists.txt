cmake_minimum_required (VERSION 2.6)

find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_subdirectory(libsysapi)
add_subdirectory(libbinstream)
add_subdirectory(libinitd-common)
add_subdirectory(libinitd-config)
add_subdirectory(libinitd-readconfig)
add_subdirectory(libinitd)
add_subdirectory(init)
add_subdirectory(initctl)
add_subdirectory(test-config)
