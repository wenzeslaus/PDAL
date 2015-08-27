#
# Python plugin CMake configuration
#

add_subdirectory(filters)
if (WITH_TESTS)
    PDAL_ADD_TEST(plangtest FILES ./test/PLangTest.cpp
        LINK_WITH ${PLANG_LIB_NAME})
endif()
