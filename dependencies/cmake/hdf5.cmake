# download and extract HDF5 library
# default configure, build and install is applied
# will be included in the project by CMake

include (ExternalProject)
set(hdf5_URL 
https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8/hdf5-1.8.19/src/CMake-hdf5-1.8.19.zip
https://www.uibk.ac.at/umwelttechnik/softwareanddatasets/mirror/cmake-hdf5-1.8.19.zip)

# Tell hdf5 that we are manually overriding certain settings
set(HDF5_CMAKE_ARGS "-DHDF5_DISABLE_COMPILER_WARNINGS:BOOL=ON" "-DBUILD_TESTING:BOOL=OFF" "-DHDF5_BUILD_EXAMPLES:BOOL=OFF" "-DHDF5_BUILD_TOOLS:BOOL=OFF" "-DHDF5_EXTERNALLY_CONFIGURED:BOOL=ON" "-DBUILD_SHARED_LIBS:BOOL=OFF" "-DBUILD_STATIC_EXECS:BOOL=ON" "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}")
message("HDF5_CMAKE_ARGS = ${HDF5_CMAKE_ARGS}")
ExternalProject_Add(hdf5
    PREFIX hdf5
    URL ${hdf5_URL}
    DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
    CMAKE_CACHE_ARGS ${HDF5_CMAKE_ARGS}
    SOURCE_SUBDIR      hdf5-1.8.19
)

set(HDF5_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
message("HDF5_INCLUDE_DIR = ${HDF5_INCLUDE_DIR}")
if (WIN32)
    set(HDF5_LIB_NAME libhdf5)
    set(HDF5_DEBUG_LIB_NAME libhdf5_D)
else()
    set(HDF5_LIB_NAME hdf5-static)
    set(HDF5_DEBUG_LIB_NAME ${HDF5_LIB_NAME})
endif()
