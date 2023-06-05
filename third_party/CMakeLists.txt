cmake_minimum_required(VERSION 3.13)

project(third_party)

include(cmake/build_target.cmake)

if(BUILD_DEPENDENCY)
    build_target(openssl)
    build_target(cares)
    build_target(protobuf)
    build_target(absl)
    build_target(re2)
    build_target(grpc)
endif()

