cmake_minimum_required(VERSION 2.8)

if (NOT BUILD_IN_METTANODE)
    include(cmake/setup.cmake)
endif()

include_directories(include)
add_subdirectory(lib)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif (BUILD_TESTING)
