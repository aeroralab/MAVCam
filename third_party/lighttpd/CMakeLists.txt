cmake_minimum_required(VERSION 3.14)

set(LIGHTTPD_VERSION 1.4.71)

project(external-lighttpd)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DBUILD_STATIC=ON"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"lighthttpd\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    lighttpd
    URL https://download.lighttpd.net/lighttpd/releases-1.4.x/lighttpd-${LIGHTTPD_VERSION}.tar.gz
    URL_HASH MD5=c1c130c0d2abafbcd9b003ffdf0d2034
    PREFIX lighttpd
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/0001-disable-LIGHTTPD_STATIC-flags-for-static-build.patch
    CMAKE_ARGS "${CMAKE_ARGS}" 
    )
