cmake_minimum_required (VERSION 2.6)

project(LIBSYSAPI)

add_library(libsysapi
   file_descriptor.cpp
   file_descriptor.h
   epoll.cpp
   epoll.h
   signalfd.cpp
   signalfd.h
   function_queue.cpp
   function_queue.h
   errors.cpp
   errors.h
   loop_device.cpp
   loop_device.h
   current_directory.cpp
   current_directory.h
)
