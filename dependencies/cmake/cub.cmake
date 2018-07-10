# simply download and extract CUDA CUB library
# will be included in the project by CMake

include (ExternalProject)

set(cub_URL 
https://github.com/NVlabs/cub/archive/1.8.0.zip
https://www.uibk.ac.at/umwelttechnik/softwareanddatasets/mirror/cub-1.8.0.zip)

ExternalProject_Add(cub
    PREFIX cub
    URL ${cub_URL}
    DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
)