cmake_minimum_required(VERSION 3.2)

# Define library. Only source files here!
project(configuration VERSION 0.1 LANGUAGES CXX)

file(GLOB_RECURSE HDRS "include/*.h")
file(GLOB_RECURSE SRCS "src/*.[hc]pp")

add_library(configuration STATIC ${HDRS} ${SRCS})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(configuration PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

# If we have compiler requirements for this library, list them
# here
#target_compile_features(configuration
#    PUBLIC cxx_auto_type
#    PRIVATE cxx_variadic_templates)

# Depend on a library that we defined in the top-level file
#target_link_libraries(configuration)

# 'make install' to the correct location
install(TARGETS configuration DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into <prefix>/cmake.
#install(EXPORT SphaseLibraryConfig DESTINATION share/SphaseLibrary/cmake)

# This makes the project importable from the build directory
#export(TARGETS configuration FILE SphaseLibraryConfig.cmake)
