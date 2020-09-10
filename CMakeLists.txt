cmake_minimum_required(VERSION 3.0)
project(LaserDiodeDriver LANGUAGES CXX)

option(MMROOT "MMROOT" ../../)

# Fetch MMDevice source
file(GLOB MMDEVSRC
       ${MMROOT}/DeviceAdapters/../MMDevice/*.cpp)

add_library(mmgr_dal_LaserDiodeDriver SHARED LaserDiodeDriver.cpp K8061.cpp DummyBoard.cpp ${MMDEVSRC})

target_link_libraries(mmgr_dal_LaserDiodeDriver comedi)
target_include_directories(mmgr_dal_LaserDiodeDriver PUBLIC ${MMROOT}/MMDevice)