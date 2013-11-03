option (UPNPC_BUILD_STATIC "Build static library" TRUE)
option (UPNPC_BUILD_SHARED "Build shared library" FALSE)
option (NO_GETADDRINFO "Define NO_GETADDRINFO" FALSE)
mark_as_advanced (NO_GETADDRINFO)

add_definitions(-DMINIUPNPC_VERSION_STRING=\"1.7\")
add_definitions(-DOS_STRING=\"${CMAKE_SYSTEM_NAME}\")

add_definitions(-DDEBUG)

if (NO_GETADDRINFO)
    add_definitions(-DNO_GETADDRINFO)
endif (NO_GETADDRINFO)

if (NOT WIN32)
    add_definitions(-DMINIUPNPC_SET_SOCKET_TIMEOUT)
    add_definitions(-D_BSD_SOURCE -D_POSIX_C_SOURCE=1)
else (NOT WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501) # XP or higher for getnameinfo and friends
endif (NOT WIN32)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-DMACOSX -D_DARWIN_C_SOURCE)
endif ()

set (MINIUPNPC_SOURCES
    igd_desc_parse.c
    miniupnpc.c
    minixml.c
    minisoap.c
    miniwget.c
    upnpcommands.c
    upnpreplyparse.c
    upnperrors.c
    connecthostport.c
    portlistingparse.c
    receivedata.c)

if (NOT WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "AmigaOS")
    list (APPEND MINIUPNPC_SOURCES minissdpc.c)
endif (NOT WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "AmigaOS")

if (WIN32)
    set_source_files_properties (${MINIUPNPC_SOURCES} PROPERTIES
                                                      COMPILE_DEFINITIONS STATICLIB
                                                      COMPILE_DEFINITIONS MINIUPNP_EXPORTS)
endif (WIN32)

if (WIN32)
    find_library (WINSOCK2_LIBRARY NAMES ws2_32 WS2_32 Ws2_32)
    find_library (IPHLPAPI_LIBRARY NAMES iphlpapi)
    set (LDLIBS ${WINSOCK2_LIBRARY} ${IPHLPAPI_LIBRARY} ${LDLIBS})
endif (WIN32)

if (NOT UPNPC_BUILD_STATIC AND NOT UPNPC_BUILD_SHARED)
    message (FATAL "Both shared and static libraries are disabled!")
endif (NOT UPNPC_BUILD_STATIC AND NOT UPNPC_BUILD_SHARED)

if (UPNPC_BUILD_STATIC)
    add_library (upnpc STATIC ${MINIUPNPC_SOURCES})
endif (UPNPC_BUILD_STATIC)

if (UPNPC_BUILD_SHARED)
    add_library (upnpc-shared SHARED ${MINIUPNPC_SOURCES})
    set_target_properties (upnpc-shared PROPERTIES OUTPUT_NAME "miniupnpc")
    set_target_properties (upnpc-shared PROPERTIES VERSION ${MINIUPNPC_VERSION})
    set_target_properties (upnpc-shared PROPERTIES SOVERSION ${MINIUPNPC_API_VERSION})
    target_link_libraries (upnpc-shared ${LDLIBS})
endif (UPNPC_BUILD_SHARED)
