cmake_minimum_required (VERSION 2.6)

project(LIBSYSAPI)

add_library(libsysapi
   file_descriptor.cpp
   file_descriptor.h
   errors.cpp
   errors.h
   loop_device.cpp
   loop_device.h
)
