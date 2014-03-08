cmake_minimum_required (VERSION 2.6)

project(TEST_CONFIG)

find_package(Boost 1.48 COMPONENTS unit_test_framework filesystem system REQUIRED)

configure_file("${PROJECT_SOURCE_DIR}/tests_directory.h.in"
               "${PROJECT_BINARY_DIR}/tests_directory.h")

include_directories("${LIBINITD_COMMON_SOURCE_DIR}"
                    "${LIBINITD_CONFIG_SOURCE_DIR}"
                    "${LIBINITD_READCONFIG_SOURCE_DIR}"
                    "${LIBSYSAPI_SOURCE_DIR}"
                    "${PROJECT_BINARY_DIR}"
                    "${Boost_INCLUDE_DIR}")

add_executable(test-config
   main.cpp
)

target_link_libraries(test-config initd-readconfig initd-config sysapi ${Boost_LIBRARIES})
