cmake_minimum_required (VERSION 2.6)

find_package(Boost 1.48 COMPONENTS unit_test_framework REQUIRED)

include_directories("${LIBINITD_SOURCE_DIR}")
include_directories("${LIBINITD_COMMON_SOURCE_DIR}")
include_directories("${LIBINITD_CONFIG_SOURCE_DIR}")
include_directories("${Boost_INCLUDE_DIR}")

add_executable(initd-test
   main.cpp
)

target_link_libraries(initd-test libinitd libinitd-config ${Boost_LIBRARIES})
