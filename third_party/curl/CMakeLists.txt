cmake_minimum_required(VERSION 3.14)

project(external-curl)

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
    "-DCURL_USE_OPENSSL=OFF"
    "-DCURL_USE_LIBSSH2=OFF"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCURL_CA_PATH_SET=OFF"
    "-DCURL_ZLIB=OFF"
    "-DHTTP_ONLY=ON"
    "-DHAVE_POSIX_STRERROR_R=1"
    "-DBUILD_TESTING=OFF"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"curl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    curl
    URL https://github.com/curl/curl/releases/download/curl-7_88_0/curl-7.88.0.tar.gz
    URL_HASH MD5=f59fd1626bd6af4b715186b443e3ab7e
    PREFIX curl
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
