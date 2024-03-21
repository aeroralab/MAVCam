cmake_minimum_required(VERSION 3.14)

project(external-grpc)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)

SET(GRPC_VERSION 1.61.1)

list(APPEND CMAKE_ARGS
"-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_STANDARD=17"
    "-DRUN_HAVE_STD_REGEX=0"
    "-DRUN_HAVE_POSIX_REGEX=0"
    "-DRUN_HAVE_STEADY_CLOCK=0"
    "-DgRPC_BUILD_TESTS=OFF"
    "-DgRPC_ZLIB_PROVIDER=package"
    "-DgRPC_CARES_PROVIDER=package"
    "-DgRPC_PROTOBUF_PROVIDER=package"
    "-DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG"
    "-DgRPC_RE2_PROVIDER=package"
    "-DgRPC_SSL_PROVIDER=package"
    "-DgRPC_ABSL_PROVIDER=package"
    "-DgRPC_INSTALL_default=ON"
    "-DgRPC_MSVC_STATIC_RUNTIME=OFF"
    "-DgRPC_BUILD_CSHARP_EXT=OFF"
    "-DgRPC_USE_SYSTEMD=OFF"
    )

if(CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_ARGS
        "-DgRPC_BUILD_CODEGEN=OFF"
        )
endif()

if(MANYLINUX1)
    list(APPEND CMAKE_ARGS
        "-DCMAKE_CXX_FLAGS=-DGPR_MANYLINUX1=1"
        )
endif()

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"grpc\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    grpc
    URL https://github.com/grpc/grpc/archive/v${GRPC_VERSION}.tar.gz
    URL_HASH MD5=ebc2bfbf6deee40c908a6e8b8e2dd191
    PREFIX grpc
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/0001-CMake-Add-gRPC_USE_SYSTEMD-option.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
