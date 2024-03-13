#
# Toolchain file for Linux on ARM hard float (armhf) architecture
#
# Tested with the following cross-compilers:
#
# - Linaro 4.8-2014.04:
#   http://releases.linaro.org/14.04/components/toolchain/binaries/
#
# ---------------------------------------------------------------
# ---------------------------------------------------------------
#
# IMPORTANT NOTE:
#
# On Ubuntu 14.04 LTS armhf I ran into linker issues with libc.so
# and libpthread.so. Those files need to be patched in your mounted
# rootfs prior to cross-compiling. Just to be clear, do not patch
# the files on your embedded system -- they are fine. Said patches
# are provided (as unified diffs) in `patches_arm-linux-gnueabihf`.
# To apply:
#
# $ cd /mnt/rootfs/usr/lib/arm-linux-gnueabihf
# $ sudo patch -b libc.so libc.so.patch
# $ sudo patch -b libpthread.so libpthread.so.patch
#
# Backup files are made as `.orig` in the same directory.
#
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(TOOLCHAIN_DIR "/opt/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/" CACHE STRING "The Toolchain root directory")
set(CROSS_GNU_TRIPLE "aarch64-none-linux-gnu" CACHE STRING "The GNU triple of the toolchain to use")
set(CMAKE_LIBRARY_ARCHITECTURE aarch64-linux-gnu)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
set(CMAKE_SYSROOT "/opt/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc")

include("${CMAKE_CURRENT_LIST_DIR}/common.toolchain.cmake")

#option(CONFIG_AR9341SDK "Artosyn AR9341 SDK" ON)
