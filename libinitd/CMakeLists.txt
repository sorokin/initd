cmake_minimum_required (VERSION 2.6)

project(LIBINITD)

include_directories("${LIBINITD_SOURCE_DIR}")
include_directories("${LIBINITD_COMMON_SOURCE_DIR}")
include_directories("${LIBINITD_CONFIG_SOURCE_DIR}")
include_directories("${LIBSYSAPI_SOURCE_DIR}")

add_library(libinitd
   initd_state.cpp  
   initd_state.h
   task.cpp
   task.h
   task_handle.cpp
   task_handle.h
   task_handle_fwd.h
   task_queue.cpp
   task_queue.h
   tasks/null_task.cpp
   tasks/null_task.h
   tasks/failed_task.cpp
   tasks/failed_task.h
   tasks/hostname_task.cpp
   tasks/hostname_task.h
   tasks/mount_task.cpp
   tasks/mount_task.h
   tasks/start_stop_task.cpp
   tasks/start_stop_task.h
)

target_link_libraries(libinitd libsysapi)
