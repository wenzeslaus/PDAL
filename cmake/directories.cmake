if(NOT ROOT_DIR)
    message(FATAL_ERROR "ROOT_DIR must be set in top-level CMakeLists.txt")
endif()

set(PDAL_CMAKE_DIR ${ROOT_DIR}/cmake)
set(PDAL_FILTER_DIR ${ROOT_DIR}/src/filters)
set(PDAL_INCLUDE_DIR ${ROOT_DIR}/include)
set(PDAL_IO_DIR ${ROOT_DIR}/src/io)
set(PDAL_KERNEL_DIR ${ROOT_DIR}/src/kernels)
set(PDAL_MODULE_DIR ${PDAL_CMAKE_DIR}/modules)
set(PDAL_TOOLS_DIR ${ROOT_DIR}/tools)
set(PDAL_UTIL_DIR ${ROOT_DIR}/src/util)

