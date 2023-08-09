cmake_minimum_required(VERSION 3.14)

project(external-tinyxml2)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DBUILD_TESTS=OFF"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"tinyxml\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    tinyxml2
    URL https://github.com/leethomason/tinyxml2/archive/refs/tags/9.0.0.tar.gz
    URL_HASH MD5=afecd941107a8e74d3d1b4363cf52bd7
    PREFIX tinyxml2
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/cmake-3.10.2.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
