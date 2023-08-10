cmake_minimum_required(VERSION 3.12.0)

set(QT_CREATOR_SKIP_VCPKG_SETUP ON)

option(USE_VCPKG_VIA_FETCH_CONTENT "Uses FetchContent to get vcpkg. You can disable that to get the dependencies yourself." ON)
if (USE_VCPKG_VIA_FETCH_CONTENT)
    include(FetchContent)
    FetchContent_Declare(
        vcpkg
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
        GIT_TAG c9fa965c2a1b1334469b4539063f3ce95383653c # 2023-07-10
    )
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        FetchContent_Populate(vcpkg)
        set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake")
    endif()
endif()

# vcpkg integration
#if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#  set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
#elseif(DEFINED CMAKE_TOOLCHAIN_FILE AND NOT CMAKE_TOOLCHAIN_FILE MATCHES ".+/vcpkg/scripts/buildsystems/vcpkg.cmake$")
#  message(STATUS "CMAKE_TOOLCHAIN_FILE is already set to '${CMAKE_TOOLCHAIN_FILE}', replace it with the vcpkg one and set VCPKG_CHAINLOAD_TOOLCHAIN_FILE to original CMAKE_TOOLCHAIN_FILE")
#  set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
#  set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")
#else()
#  message(STATUS "CMAKE_TOOLCHAIN_FILE is already set to '${CMAKE_TOOLCHAIN_FILE}'")
#  message(STATUS "VCPKG_CHAINLOAD_TOOLCHAIN_FILE is already set to '${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}'")
#endif()

message("VCPKG_INSTALL_OPTIONS: ${VCPKG_INSTALL_OPTIONS}")
if(APPLE)
    set(VCPKG_APPLOCAL_DEPS OFF)
endif()

project(lichtsteuerung VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up Qt stuff:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6
  COMPONENTS Qml Quick Network Widgets NetworkAuth WebSockets Mqtt Core5Compat Svg
  REQUIRED
)


add_subdirectory(src)
