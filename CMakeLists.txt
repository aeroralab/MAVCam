cmake_minimum_required(VERSION 3.13)

option(BUILD_DEPENDENCY "Build dependencies" ON)
option(BUILD_SHARED_LIBS "Build core as shared libraries instead of static ones" ON)

project(middleware)

set(DEPS_BUILD_PATH "${PROJECT_BINARY_DIR}/third_party" CACHE PATH "build path for the dependencies.")
set(DEPS_INSTALL_PATH "${DEPS_BUILD_PATH}/install" CACHE PATH "Install path for the dependencies.")

add_subdirectory(third_party)
add_subdirectory(src)