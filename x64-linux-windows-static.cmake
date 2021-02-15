# from https://stackoverflow.com/questions/58777810/how-to-integrate-vcpkg-in-linux-with-cross-build-toolchain-as-well-as-sysroot

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME MinGW)

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE /usr/src/mxe/usr/x86_64-w64-mingw32.shared.posix/share/cmake/mxe-conf.cmake)
