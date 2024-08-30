cmake_minimum_required(VERSION 3.21.0)

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

if(APPLE)
    set(VCPKG_APPLOCAL_DEPS OFF)
endif()

project(lichtsteuerung VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(WITH_QtMqtt "Build with MQTT support. QtMqtt must be installed" OFF)

find_package(Qt6
  COMPONENTS Qml Quick Network Widgets NetworkAuth WebSockets Core5Compat Svg
  REQUIRED
)

if (WITH_QtMqtt)
    find_package(Qt6 COMPONENTS Mqtt REQUIRED)
endif()


add_subdirectory(src)
