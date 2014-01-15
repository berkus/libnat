add_subdirectory(libupnp)

create_test(natpmp LIBS nat arsenal ${Boost_LIBRARIES})
create_test(upnp LIBS nat upnpc arsenal ${Boost_LIBRARIES})
