add_subdirectory(libupnp)

create_test(natpmp LIBS nat support ${Boost_LIBRARIES})
create_test(upnp LIBS nat upnpc support ${Boost_LIBRARIES})
