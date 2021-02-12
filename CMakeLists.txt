cmake_minimum_required(VERSION 3.12.0)

# vcpkg integration
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(lichtsteuerung VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up Qt stuff:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5
  COMPONENTS Qml Quick Network Widgets NetworkAuth WebSockets
  REQUIRED
)


add_subdirectory(src)
