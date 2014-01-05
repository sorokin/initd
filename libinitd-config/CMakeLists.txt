cmake_minimum_required (VERSION 2.6)

project(LIBINITD_CONFIG)

include_directories("${LIBINITD_COMMON_SOURCE_DIR}")

add_library(libinitd-config
   null_task_data.h
   hostname_task_data.h
   mount_task_data.h
   mount_options.cpp
   mount_options.h
   control_task_data.h
   start_stop_task_data.h
   start_stop_task_data.cpp
   task_data.h
   task_data.cpp
   task_description_fwd.h
   task_description.h
   task_description.cpp
   task_descriptions.h
   task_descriptions.cpp
   run_level.h
   run_level.cpp
)

target_link_libraries(libinitd-config)
