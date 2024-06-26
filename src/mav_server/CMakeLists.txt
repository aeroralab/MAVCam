
project(mav_server)
set(EXECUTABLE_NAME mav_server)

set(MAV_SERVER_SOURCES
    mav_server_bin.cpp
    mav_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/mavsdk_options.grpc.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/mavsdk_options.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/camera/camera.grpc.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/camera/camera.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/camera/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/camera/camera_impl.cpp
)

add_executable(${EXECUTABLE_NAME}
    ${MAV_SERVER_SOURCES}
)

target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${DEP_INSTALL_DIR}/include
)

find_package(OpenSSL REQUIRED)

if (BUILD_QCOM)
    set(gRPC_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/grpc)
    set(Protobuf_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/protobuf)
    set(absl_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/absl)
    set(re2_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/re2)
    set(c-ares_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/c-ares)
endif()

find_package(gRPC REQUIRED)

target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE
    base
    gRPC::grpc++
)

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

