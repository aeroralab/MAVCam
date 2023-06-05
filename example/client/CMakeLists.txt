cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(client)

add_executable(client
    client.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(client
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera_server PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera_server PRIVATE -WX -W2)
endif()
