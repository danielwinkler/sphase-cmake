# download and extract H5Part library
# folder will be patched with CMakeLists.txt
# default configure, build and install is applied
# will be included in the project by CMake

include (ExternalProject)

set(h5part_URL https://codeforge.lbl.gov/frs/download.php/file/387/H5Part-1.6.6.tar.gz https://www.uibk.ac.at/umwelttechnik/softwareanddatasets/mirror/H5Part-1.6.6.tar.gz)
set(h5part_src_dir ${CMAKE_CURRENT_BINARY_DIR}/h5part/src/h5part)

set(h5part_CMAKE_ARGS "-DHDF5_INCLUDE_DIR:PATH=${HDF5_INCLUDE_DIR}"
"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}")
message("h5part_CMAKE_ARGS = ${h5part_CMAKE_ARGS}")
ExternalProject_Add(h5part
    PREFIX h5part
    URL ${h5part_URL}
    DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
    CMAKE_CACHE_ARGS ${h5part_CMAKE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/patches/h5part/CMakeLists.txt ${h5part_src_dir}
    DEPENDS hdf5
)

set(H5Part_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
message("H5Part_INCLUDE_DIR = ${H5Part_INCLUDE_DIR}")