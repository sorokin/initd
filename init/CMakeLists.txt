cmake_minimum_required (VERSION 2.6)

project(INIT)

find_package(Boost 1.48)

include_directories("${LIBINITD_SOURCE_DIR}"
                    "${LIBINITD_COMMON_SOURCE_DIR}"
                    "${LIBINITD_CONFIG_SOURCE_DIR}"
                    "${LIBINITD_READCONFIG_SOURCE_DIR}"
                    "${LIBSYSAPI_SOURCE_DIR}"
                    "${Boost_INCLUDE_DIR}")

add_executable(init
   initd-main.cpp
)

target_link_libraries(init initd initd-readconfig initd-config sysapi ${CMAKE_THREAD_LIBS_INIT})
