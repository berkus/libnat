add_subdirectory(libupnp)
add_library(nat STATIC
    natpmpprotocol.cc
    natpmpclient.cc
    natpmpclientimpl.cc
    upnpclient.cc
    miniupnpclientimpl.cc)
