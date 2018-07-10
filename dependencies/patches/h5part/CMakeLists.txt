cmake_minimum_required(VERSION 3.2)

# Define library. Only source files here!
project(H5Part VERSION 0.1 LANGUAGES C CXX)

#execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/configure"
#                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

#--------------------------------------------------------
# Source files
#--------------------------------------------------------
file(GLOB H5Part_HDRS src/*.h)
file(GLOB H5Part_SRCS src/*.c)

#
# Make sure msvc builds as cxx not C
# This resolves some struct typedef issues
# UPDATE: in VS 2015 I had to remove this
#
# IF(WIN32)
#   SET_SOURCE_FILES_PROPERTIES(
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/H5Part.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/H5Block.c
#     PROPERTIES LANGUAGE CXX
#   )
# ENDIF (WIN32)

#--------------------------------------------------------
# List the libraries used
#--------------------------------------------------------
SET(H5Part_LIBS hdf5-static z dl m)

#--------------------------------------------------------
# Include class, configuration headers
#--------------------------------------------------------
SET(H5PART_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${HDF5_INCLUDE_DIR}
)

INCLUDE_DIRECTORIES(${H5PART_INCLUDE_DIRS})

#--------------------------------------------------------
# Create the Library
#--------------------------------------------------------
ADD_LIBRARY(H5Part STATIC
  ${H5Part_SRCS}
)

TARGET_LINK_LIBRARIES(H5Part
  PUBLIC
    ${H5Part_LIBS}
)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(H5Part PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>)

install(FILES ${H5Part_HDRS} DESTINATION include)

install(TARGETS H5Part DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# Make sure static lib can be linked into dynamic one
IF(BUILD_SHARED_LIBS AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_COMPILER_IS_GNUCC)
  SET_TARGET_PROPERTIES( H5Part PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(BUILD_SHARED_LIBS AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_COMPILER_IS_GNUCC)
