cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(client_example)

message(STATUS "build client example")

add_executable(client_example
    client.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(client_example
    MAVSDK::mavsdk
)
