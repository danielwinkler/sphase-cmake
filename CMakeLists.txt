cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

project(sphase VERSION 0.1 LANGUAGES CXX)

# Set C++11 as standard for the whole project
set(CMAKE_CXX_STANDARD 11)

set(BUILD_GPU_ARCH "52" CACHE STRING "Define the CUDA architecture")
# this was necessary with an older cmake version, maybe now its fine
# I leave it here just in case
option(BUILD_GPU_FORCE_INLINES "CUDA 7.5 AND Ubuntu 16.04 needs this enabled" OFF)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "default install path" FORCE )
    set (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OFF)
endif()

# Location where external projects will be downloaded
set (DOWNLOAD_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/downloads"
     CACHE PATH "Location where external projects will be downloaded.")
mark_as_advanced(DOWNLOAD_LOCATION)

# External dependencies
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/dependencies/cmake)
include(cub)
include(tclap)
include(hdf5)
include(h5part)
ExternalProject_Get_property(cub SOURCE_DIR)
set(CUB_SOURCE_DIR ${SOURCE_DIR})
ExternalProject_Get_property(tclap SOURCE_DIR)
set(TCLAP_SOURCE_DIR ${SOURCE_DIR}/include)


if ( WIN32 )
    # list(APPEND CMAKE_PREFIX_PATH "C:/Qt/5.5/msvc2013_64/")
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
endif ()

message(STATUS ${CMAKE_CXX_COMPILER_ID})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})
# Add compiler flags for building executables (-fPIE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")

# Targets that we develop
add_subdirectory(configuration)
add_subdirectory(gpusphase)
