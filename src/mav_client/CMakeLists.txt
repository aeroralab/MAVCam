
project(mav_client)

set(EXECUTE_NAME mav_client)

set(MAV_CLIENT_SOURCES
    mav_client_bin.cpp
    mav_client.cpp
    camera_client.cpp
    camera_local_client.cpp
)

if (BUILD_SERVER)
    set(MAV_CLIENT_SOURCES
        ${MAV_CLIENT_SOURCES}
        camera_rpc_client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/mavsdk_options.grpc.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/mavsdk_options.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/camera/camera.grpc.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/camera/camera.pb.cc)
endif()

add_executable(${EXECUTE_NAME}
    ${MAV_CLIENT_SOURCES}
)

target_include_directories(${EXECUTE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${DEP_INSTALL_DIR}/include
)

find_package(MAVSDK REQUIRED)

target_link_libraries(${EXECUTE_NAME}
    PRIVATE
    base
    MAVSDK::mavsdk
)

if (BUILD_SERVER)
    target_include_directories(${EXECUTE_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated
    )

    list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

    find_package(OpenSSL REQUIRED)
    find_package(gRPC REQUIRED)

    target_link_libraries(${EXECUTE_NAME}
        PRIVATE
        gRPC::grpc++
    )
endif()

install(IMPORTED_RUNTIME_ARTIFACTS MAVSDK::mavsdk DESTINATION "./") 
install(TARGETS ${EXECUTE_NAME} RUNTIME DESTINATION "./")

