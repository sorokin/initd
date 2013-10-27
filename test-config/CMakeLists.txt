cmake_minimum_required (VERSION 2.6)

find_package(Boost 1.48 COMPONENTS unit_test_framework filesystem system REQUIRED)

include_directories("${LIBINITD_COMMON_SOURCE_DIR}")
include_directories("${LIBINITD_CONFIG_SOURCE_DIR}")
include_directories("${LIBINITD_READCONFIG_SOURCE_DIR}")
include_directories("${LIBSYSAPI_SOURCE_DIR}")
include_directories("${Boost_INCLUDE_DIR}")

add_executable(initd-config-test
   main.cpp
)

target_link_libraries(initd-config-test initd-readconfig libinitd-config libsysapi ${Boost_LIBRARIES})
