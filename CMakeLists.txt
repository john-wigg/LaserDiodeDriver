cmake_minimum_required(VERSION 3.1)
project(LaserDiodeDriver LANGUAGES CXX)

set(MMROOT "mmCoreAndDevices" CACHE STRING "(Relative or absolute) path to mmCoreAndDevices directory including the directory itself.")

# Fetch MMDevice source
file(GLOB MMDEVSRC
       ${MMROOT}/MMDevice/*.cpp)

# Define library and core sources
add_library(mmgr_dal_LaserDiodeDriver SHARED LaserDiodeDriver.cpp ${MMDEVSRC})

# Add include directories
target_include_directories(mmgr_dal_LaserDiodeDriver PUBLIC ${MMROOT}/MMDevice)

# Check which interface boards are compatible with the current platform.
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
       set(BUILD_ARDUINO true)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
       set(BUILD_ARDUINO true)
       target_compile_definitions(mmgr_dal_LaserDiodeDriver PUBLIC MODULE_EXPORTS) # required on windows
else()
       message(FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} is currently not supported.")
endif()

# Add sources for specific interface boards
if (BUILD_ARDUINO)
       target_compile_definitions(mmgr_dal_LaserDiodeDriver PUBLIC -DBUILD_ARDUINO)
       target_sources(mmgr_dal_LaserDiodeDriver PRIVATE Arduino.cpp)
       add_subdirectory(vendor/serial)
       target_link_libraries(mmgr_dal_LaserDiodeDriver PRIVATE serial)
       target_include_directories(serial PUBLIC vendor/serial/include)
       if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
              target_compile_options(serial PRIVATE -fPIC) # Needed to build shared library on GCC
       endif()
endif()

# Disable precompiler warnigs for functions such as sscanf() in MSVC
target_compile_definitions(mmgr_dal_LaserDiodeDriver PRIVATE -D_CRT_SECURE_NO_WARNINGS)