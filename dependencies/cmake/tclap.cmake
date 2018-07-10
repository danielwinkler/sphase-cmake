# simply download and extract TCLAP library
# will be included in the project by CMake

include (ExternalProject)

set(tclap_URL https://www.uibk.ac.at/umwelttechnik/softwareanddatasets/mirror/tclap-1.2.1.zip)

ExternalProject_Add(tclap
    PREFIX tclap
    URL ${tclap_URL}
    DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
)