cmake_minimum_required (VERSION 2.6)

find_package(Boost 1.48 COMPONENTS system REQUIRED)
find_package(Threads)

include_directories("${LIBINITD_SOURCE_DIR}")
include_directories("${LIBINITD_COMMON_SOURCE_DIR}")
include_directories("${LIBINITD_CONFIG_SOURCE_DIR}")
include_directories("${LIBINITD_READCONFIG_SOURCE_DIR}")
include_directories("${LIBSYSAPI_SOURCE_DIR}")
include_directories("${Boost_INCLUDE_DIR}")

add_executable(initd
   initd-main.cpp
)

target_link_libraries(initd libinitd libinitd-config initd-readconfig libsysapi ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
