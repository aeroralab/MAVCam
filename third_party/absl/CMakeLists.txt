cmake_minimum_required(VERSION 3.14)

project(external-absl)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)

set(ABSEIL_CPP_VERSION 20240116.1)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_STANDARD=17"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"absl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    absl
    URL https://github.com/abseil/abseil-cpp/archive/refs/tags/${ABSEIL_CPP_VERSION}.tar.gz
    URL_HASH MD5=fdff5a0a90a8ffcded7dee1c08f02a6c
    PREFIX absl
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/0001-Fix-GCC7-C-17-build.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
