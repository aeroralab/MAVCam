# For more information, see 
# https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html,
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(BUILD_TARGET rv1106)

set(TOOLCHAIN_DIR "/opt/toolchain/arm-rockchip830-linux-uclibcgnueabihf" CACHE STRING "The toolchain root directory")
set(CROSS_GNU_TRIPLE "arm-rockchip830-linux-uclibcgnueabihf" CACHE STRING "The GNU triple of the toolchain to use")
set(CMAKE_LIBRARY_ARCHITECTURE arm-rockchip830-linux-uclibcgnueabihf)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")

# compiler flags
set(CMAKE_C_FLAGS_INIT       "-march=armv7-a -mfpu=neon -mfloat-abi=hard -g -Os -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_INIT     "-march=armv7-a -mfpu=neon -mfloat-abi=hard -g -Os -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set(CMAKE_Fortran_FLAGS_INIT "-march=armv7-a -mfpu=neon -mfloat-abi=hard -g -Os -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")

include("${CMAKE_CURRENT_LIST_DIR}/common.toolchain.cmake")
