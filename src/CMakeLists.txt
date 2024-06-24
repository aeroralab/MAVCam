cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/compiler_flags.cmake)

option(BUILD_SERVER "Build server and client with grpc support" ON)

add_subdirectory(base)
add_subdirectory(mav_client)
if (BUILD_SERVER)
    add_compile_definitions(ENABLE_SERVER)
    add_subdirectory(mav_server)
endif()

#install definition file
set(INSTALL_DESTINATION $(CMAKE_INSTALL_PREFIX)/definition)
set(DEFINITION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/definition)

install(DIRECTORY ${DEFINITION_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

