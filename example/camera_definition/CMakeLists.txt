cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_definition)

message(STATUS "build camera definition example")

add_executable(${PROJECT_NAME}
    camera_definition.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(${PROJECT_NAME}
    MAVSDK::mavsdk
)
