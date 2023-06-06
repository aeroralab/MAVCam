
project(mid_client)

set(MID_CLIENT_SOURCES
    mid_client_bin.cpp
    mid_client.cpp
    camera_client.cpp
    camera_local_client.cpp
    camera_rpc_client.cpp
)

set(EXECUTABLE_NAME mid_client)

add_executable(${EXECUTABLE_NAME}
    ${MID_CLIENT_SOURCES}
)

target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

find_package(MAVSDK REQUIRED)

set(CMAKE_MODULE_PATH "${${CMAKE_BINARY_DIR}}/third_party/install;${CMAKE_MODULE_PATH}")
# find_package(OpenSSL REQUIRED)
# find_package(gRPC REQUIRED)

target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE
    base
    MAVSDK::mavsdk
    # gRPC::grpc++
)