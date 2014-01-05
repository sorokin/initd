cmake_minimum_required (VERSION 2.6)

project(INITCTL)

include_directories("${LIBSYSAPI_SOURCE_DIR}" "${BINSTREAM_SOURCE_DIR}")

add_executable(initctl
   main.cpp
)

target_link_libraries(initctl libsysapi binstream)
