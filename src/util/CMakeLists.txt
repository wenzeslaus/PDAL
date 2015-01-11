#
# Make sure we don't attempt to add a library more than once
#
get_property(EXISTS GLOBAL PROPERTY _UTIL_INCLUDED)
if(EXISTS)
    return()
endif()

PDAL_ADD_LIBRARY(${PDAL_UTIL_LIB_NAME} SHARED Bounds.cpp)

set_property(GLOBAL PROPERTY _UTIL_INCLUDED TRUE)

