#
# Python plugin CMake configuration
#

add_subdirectory(filters)
if (WITH_TESTS)
    include(${PDAL_CMAKE_DIR}/python.cmake)
    PDAL_ADD_TEST(plangtest FILES ./test/PLangTest.cpp
        LINK_WITH pdal_plang)
endif()
