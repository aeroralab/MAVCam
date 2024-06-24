cmake_minimum_required(VERSION 3.14)

project(external-MAVSDK)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)

set(MAVSDK_VERSION 1.5.0)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DSUPERBUILD=OFF"
    "-DBUILD_TESTS=OFF"
    "-DBUILD_WITHOUT_CURL=ON"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"MAVSDK\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

if(BUILD_QCOM)
    ExternalProject_add(
        MAVSDK
        URL https://github.com/GoerLabs/MAVSDK/archive/refs/tags/v${MAVSDK_VERSION}.tar.gz
        URL_HASH MD5=8a7d7faf4d8b9b9fbbdfeca3f15e01b8
        PREFIX MAVSDK
        DOWNLOAD_DIR "${DOWNLOAD_DIR}"
        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/0001-fixed-qcom-compile-error.patch && patch -p1 < ${PROJECT_SOURCE_DIR}/0002-manually-set-MAVSDK-version.patch
        CMAKE_ARGS "${CMAKE_ARGS}"
    )
else()
    ExternalProject_add(
        MAVSDK
        URL https://github.com/GoerLabs/MAVSDK/archive/refs/tags/v${MAVSDK_VERSION}.tar.gz
        URL_HASH MD5=8a7d7faf4d8b9b9fbbdfeca3f15e01b8
        PREFIX MAVSDK
        DOWNLOAD_DIR "${DOWNLOAD_DIR}"
        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/0002-manually-set-MAVSDK-version.patch
        CMAKE_ARGS "${CMAKE_ARGS}"
    )
endif()
