cmake_minimum_required(VERSION 3.12.0)


# vcpkg integration
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR ${CMAKE_TOOLCHAIN_FILE} STREQUAL "")
  set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(DEFINED CMAKE_TOOLCHAIN_FILE AND NOT ${CMAKE_TOOLCHAIN_FILE} MATCHES ".+/vcpkg/scripts/buildsystems/vcpkg.cmake$")
  message(STATUS "CMAKE_TOOLCHAIN_FILE is already set to '${CMAKE_TOOLCHAIN_FILE}', replace it with the vcpkg one and set VCPKG_CHAINLOAD_TOOLCHAIN_FILE to original CMAKE_TOOLCHAIN_FILE")
  set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
  set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")
else()
  message(STATUS "CMAKE_TOOLCHAIN_FILE is already set to '${CMAKE_TOOLCHAIN_FILE}'")
  message(STATUS "VCPKG_CHAINLOAD_TOOLCHAIN_FILE is already set to '${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}'")
endif()

project(lichtsteuerung VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up Qt stuff:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


#if(DEFINED ENV{GITLAB_CI}) # see https://github.com/microsoft/vcpkg/issues/17163
#  list(POP_FRONT CMAKE_FIND_ROOT_PATH MXE_PATH)
#  list(APPEND CMAKE_FIND_ROOT_PATH ${MXE_PATH})
#endif()

find_package(Qt5
  COMPONENTS Qml Quick Network Widgets NetworkAuth WebSockets
  REQUIRED
)


add_subdirectory(src)
