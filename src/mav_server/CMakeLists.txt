
project(mid_server)

set(MID_SERVER_SOURCES
    mid_server_bin.cpp
    mid_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/mavsdk_options.grpc.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/mavsdk_options.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/camera/camera.grpc.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/camera/camera.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/camera/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/camera/camera_impl.cpp
)

set(EXECUTABLE_NAME mid_server)

add_executable(${EXECUTABLE_NAME}
    ${MID_SERVER_SOURCES}
)

target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${DEP_INSTALL_DIR}/include
)

list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

find_package(OpenSSL REQUIRED)
find_package(gRPC REQUIRED)

target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE
    base
    gRPC::grpc++
)

install(TARGETS mid_server RUNTIME DESTINATION "./")

