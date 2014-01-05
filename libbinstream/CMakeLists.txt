cmake_minimum_required (VERSION 2.6)

project(LIBBINSTREAM)

add_library(binstream
   binary_stream.cpp
   binary_stream.h
   ctl_protocol.cpp
   ctl_protocol.h
)
