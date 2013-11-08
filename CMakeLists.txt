cmake_minimum_required(VERSION 2.8)

if (NOT BUILD_IN_METTANODE)
    include(cmake/setup.cmake)
    include_directories(3rdparty/endian 3rdparty/support/include 3rdparty/plist/include)
    add_subdirectory(3rdparty/plist)
    add_subdirectory(3rdparty/support/lib)
endif()

include_directories(include)
add_subdirectory(lib)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif (BUILD_TESTING)
