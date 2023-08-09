cmake_minimum_required(VERSION 3.14)

project(third_party)

include(cmake/build_target.cmake)


if(BUILD_DEPENDENCY)
    list(APPEND CMAKE_PREFIX_PATH "${DEP_INSTALL_DIR}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
    #set(PKG_CONFIG_PATH "${DEP_INSTALL_DIR}/usr/lib/pkgconfig:${DEP_INSTALL_DIR}/usr/share/pkgconfig:${DEP_INSTALL_DIR}/lib/pkgconfig:${DEP_INSTALL_DIR}/share/pkgconfig" PARENT_SCOPE)

    build_target(zlib)
    build_target(openssl)
    build_target(cares)
    build_target(absl)
    build_target(protobuf)
    build_target(re2)
    build_target(grpc)
#    build_target(MAVSDK)
endif()

