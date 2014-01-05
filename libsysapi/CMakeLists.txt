cmake_minimum_required (VERSION 2.6)

project(LIBSYSAPI)

include_directories("${LIBSYSAPI_SOURCE_DIR}")

add_library(libsysapi
   file_descriptor.cpp
   file_descriptor.h
   epoll.cpp
   epoll.h
   signalfd.cpp
   signalfd.h
   block_signals.cpp
   block_signals.h
   sigchild_handler.cpp
   sigchild_handler.h
   function_queue.cpp
   function_queue.h
   errors.cpp
   errors.h
   loop_device.cpp
   loop_device.h
   current_directory.cpp
   current_directory.h
   process.cpp
   process.h
   reboot.cpp
   reboot.h
   network/socket_address.cpp
   network/socket_address.h
   network/stream_server_socket.cpp
   network/stream_server_socket.h
   network/stream_socket.cpp
   network/stream_socket.h
   network/socket_common.cpp
   network/socket_common.h
)
