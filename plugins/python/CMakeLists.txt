#
# Python plugin CMake configuration
#

add_subdirectory(filters)
if (WITH_TESTS)
    include(${PDAL_CMAKE_DIR}/python.cmake)
    include_directories(SYSTEM ${PYTHON_INCLUDE_DIR})
    PDAL_ADD_TEST(plangtest FILES ./test/PLangTest.cpp
        LINK_WITH ${PLANG_LIB_NAME})
endif()
