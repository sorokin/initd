cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_subdirectory(libsysapi)
add_subdirectory(libinitd-common)
add_subdirectory(libinitd-config)
add_subdirectory(libinitd-readconfig)
add_subdirectory(libinitd)
add_subdirectory(initd)
add_subdirectory(test-config)
