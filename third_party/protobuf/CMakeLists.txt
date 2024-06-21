cmake_minimum_required(VERSION 3.14)

project(external-protobuf)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)

set(PROTOBUF_VERSION 25.1)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-Dprotobuf_BUILD_TESTS=OFF"
    "-Dprotobuf_BUILD_SHARED_LIBS=OFF"
    "-Dprotobuf_MODULE_COMPATIBLE=ON"
    "-Dprotobuf_MSVC_STATIC_RUNTIME=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-Dprotobuf_ABSL_PROVIDER=package"
    "-DCMAKE_CXX_STANDARD=17"
    )

if(CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_ARGS
        "-Dprotobuf_BUILD_PROTOC_BINARIES=OFF"
        )
endif()

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"protobuf\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

if(BUILD_QCOM)
    ExternalProject_add(
        protobuf
        URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-${PROTOBUF_VERSION}.tar.gz
        URL_HASH MD5=105105aae0153d60059e982ead4d1d94
        PREFIX protobuf
        DOWNLOAD_DIR "${DOWNLOAD_DIR}"
        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/0001-manually-add-abseil-cpp-find-path.patch && patch -p1 < ${PROJECT_SOURCE_DIR}/0002-manually-set-utf8_range-in-protobuf-config.cmake.in.patch
        CMAKE_ARGS "${CMAKE_ARGS}"
        )
else()
    ExternalProject_add(
        protobuf
        URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-${PROTOBUF_VERSION}.tar.gz
        URL_HASH MD5=105105aae0153d60059e982ead4d1d94
        PREFIX protobuf
        DOWNLOAD_DIR "${DOWNLOAD_DIR}"
        CMAKE_ARGS "${CMAKE_ARGS}"
        )
endif()
