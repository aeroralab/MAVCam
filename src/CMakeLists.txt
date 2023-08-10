cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/compiler_flags.cmake)

option(BUILD_SERVER "Build server and client with grpc support" ON)

add_subdirectory(base)
if (BUILD_SERVER)
    add_compile_definitions(ENABLE_SERVER)
    add_subdirectory(mid_server)
endif()
add_subdirectory(mid_client)
