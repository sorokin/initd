cmake_minimum_required (VERSION 2.6)

project(LIBINITD)

include_directories("${LIBINITD_SOURCE_DIR}"
		    "${LIBINITD_COMMON_SOURCE_DIR}"
		    "${LIBINITD_CONFIG_SOURCE_DIR}"
		    "${LIBSYSAPI_SOURCE_DIR}"
		    "${LIBBINSTREAM_SOURCE_DIR}")

add_library(initd
   initd_state.cpp
   initd_state.h
   task_context.h
   state_context.h
   tasks/async_task_handle.cpp
   tasks/async_task_handle.h
   tasks/async_task_handle_fwd.h
   tasks/sync_task_adaptor.cpp
   tasks/sync_task_adaptor.h
   tasks/async_start_stop_task.cpp
   tasks/async_start_stop_task.h
   tasks/create_async_task_handle.cpp
   tasks/create_async_task_handle.h
   tasks/synchronous/task_handle.cpp
   tasks/synchronous/task_handle.h
   tasks/synchronous/task_handle_fwd.h
   tasks/synchronous/null_task.cpp
   tasks/synchronous/null_task.h
   tasks/synchronous/hostname_task.cpp
   tasks/synchronous/hostname_task.h
   tasks/synchronous/mount_task.cpp
   tasks/synchronous/mount_task.h
   tasks/synchronous/control_task.cpp
   tasks/synchronous/control_task.h
)

target_link_libraries(initd sysapi binstream)
