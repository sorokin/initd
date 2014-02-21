cmake_minimum_required (VERSION 2.6)

project(INITCTL)

find_package(Boost 1.48)

include_directories("${LIBSYSAPI_SOURCE_DIR}"
                    "${LIBBINSTREAM_SOURCE_DIR}"
                    "${Boost_INCLUDE_DIR}")

add_executable(initctl
   main.cpp
)

target_link_libraries(initctl sysapi binstream)
