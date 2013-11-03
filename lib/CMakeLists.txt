add_subdirectory(libupnp)
add_library(nat STATIC
    natpmpprotocol.cc
    natpmpclient.cc
    natpmpclientimpl.cc
    upnpclient.cc
    miniupnpclientimpl.cc
    maidsafe_calllatertimer.cc
    maidsafe_network_interface.cc
    maidsafe_gateway.cc
    maidsafe_utils.cc)
add_definitions(-Wno-unused-function) # in maidsafe_network_interface.cc

if (UNIX AND NOT APPLE)
    add_definitions(-DMAIDSAFE_LINUX)
endif
