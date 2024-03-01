cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_operation)

message(STATUS "build camera operation example")

add_executable(${CMAKE_PROJECT_NAME}
    camera_operation.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME}
    MAVSDK::mavsdk
)
