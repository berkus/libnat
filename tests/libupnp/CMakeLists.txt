if (WIN32)
    find_library (WINSOCK2_LIBRARY NAMES ws2_32 WS2_32 Ws2_32)
    find_library (IPHLPAPI_LIBRARY NAMES iphlpapi)
    set (LDLIBS ${WINSOCK2_LIBRARY} ${IPHLPAPI_LIBRARY} ${LDLIBS})
endif (WIN32)

add_executable(upnpc-static upnpc.c)
target_link_libraries(upnpc-static nat upnpc ${LDLIBS})
install(TARGETS upnpc-static
    RUNTIME DESTINATION tools)

add_executable(testminixml testminixml.c)
target_link_libraries(testminixml nat upnpc ${LDLIBS})
install(TARGETS testminixml
    RUNTIME DESTINATION tests/nat)

add_executable(minixmlvalid minixmlvalid.c)
target_link_libraries(minixmlvalid nat upnpc ${LDLIBS})
install(TARGETS minixmlvalid
    RUNTIME DESTINATION tests/nat)

add_executable(testupnpreplyparse testupnpreplyparse.c)
target_link_libraries(testupnpreplyparse nat upnpc ${LDLIBS})
install(TARGETS testupnpreplyparse
    RUNTIME DESTINATION tests/nat)

add_executable(testigddescparse testigddescparse.c)
target_link_libraries(testigddescparse nat upnpc ${LDLIBS})
install(TARGETS testigddescparse
    RUNTIME DESTINATION tests/nat)

add_executable(testminiwget testminiwget.c)
target_link_libraries(testminiwget nat upnpc ${LDLIBS})
install(TARGETS testminiwget
    RUNTIME DESTINATION tests/nat)
