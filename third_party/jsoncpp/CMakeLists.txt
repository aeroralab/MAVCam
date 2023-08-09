cmake_minimum_required(VERSION 3.14)

project(external-jsoncpp)

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
    "-DBUILD_SHARED_LIBS=OFF"
    "-DJSONCPP_WITH_CMAKE_PACKAGE=ON"
    "-DJSONCPP_WITH_TESTS=OFF"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"jsoncpp\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    jsoncpp
    URL https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.5.tar.gz
    URL_HASH MD5=d6c8c609f2162eff373db62b90a051c7
    PREFIX jsoncpp
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
