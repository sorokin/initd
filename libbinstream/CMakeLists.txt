cmake_minimum_required (VERSION 2.6)

find_package(Boost 1.48 REQUIRED)

include_directories("${Boost_INCLUDE_DIR}")

project(LIBBINSTREAM)

add_library(binstream
   binary_stream.cpp
   binary_stream.h
   ctl_protocol.cpp
   ctl_protocol.h
)
