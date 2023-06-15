cmake_minimum_required(VERSION 3.13)

option(BUILD_DEPENDENCY "Build dependencies" ON)
option(BUILD_SHARED_LIBS "Build core as shared libraries instead of static ones" ON)

project(middleware)

set(DEPS_BUILD_PATH "${PROJECT_BINARY_DIR}/third_party" CACHE PATH "build path for the dependencies.")
set(DEPS_INSTALL_PATH "${DEPS_BUILD_PATH}/install" CACHE PATH "Install path for the dependencies.")

execute_process(
    COMMAND git describe --abbrev=8 --dirty --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GITVER
)

if (NOT GITVER EQUAL "0")
    message(STATUS "No git version found. Falling back to empty version no")
    set(VERSION_STR "0.0.0")
endif()

message(STATUS "Version: ${VERSION_STR}")
STRING(REGEX REPLACE v\([0-9]+\).[0-9]+.[0-9]+.*$ \\1 VERSION_MAJOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.\([0-9]+\).[0-9]+.*$ \\1 VERSION_MINOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.[0-9]+.\([0-9]+\).*$ \\1 VERSION_PATCH "${VERSION_STR}")

message(STATUS "Version major: ${MAVCAM_VERSION_MAJOR}")
message(STATUS "Version minor: ${MAVCAM_VERSION_MINOR}")
message(STATUS "Version patch: ${MAVCAM_VERSION_PATCH}")

set(BUILD_TIME "")
string(TIMESTAMP BUILD_TIME "%Y-%m-%dT%H:%M:%S")

set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  @ONLY)
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(third_party)
add_subdirectory(src)