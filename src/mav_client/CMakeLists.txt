
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

set(MAVSDK_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/MAVSDK/)

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

    find_package(OpenSSL REQUIRED)
    if (BUILD_QCOM)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(ZLIB REQUIRED zlib)
        target_link_libraries(${EXECUTE_NAME} PRIVATE ${ZLIB_LIBRARIES})
    
        set(gRPC_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/grpc)
        set(Protobuf_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/protobuf)
        set(absl_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/absl)
        set(re2_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/re2)
        set(c-ares_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/c-ares)
    endif()
    find_package(gRPC REQUIRED)

    target_link_libraries(${EXECUTE_NAME}
        PRIVATE
        gRPC::grpc++
    )
endif()

if (NOT BUILD_QCOM)
    install(TARGETS MAVSDK::mavsdk LIBRARY DESTINATION "lib")
endif() 
install(TARGETS ${EXECUTE_NAME} RUNTIME DESTINATION "bin")

