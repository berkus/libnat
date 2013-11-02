Libraries for NAT traversal and hole punching
=============================================

Notes
=====

- NAT relaying (using accessible server S to forward packets between A and B) - TURN protocol
- Connection reversal (via server S, but while only one of A, B is behind NAT)
- UDP Hole punching (via server S)
- hairpin (loopback) translation?

Relevant documents
==================

* [NAT-PMP RFC](http://tools.ietf.org/html/rfc6886)
* [Port Control Protocol](http://tools.ietf.org/html/rfc6887)
* [UPnP](http://www.upnp.org/)

Relevant libraries
==================

* [miniupnpc](https://github.com/miniupnp/miniupnp/tree/master/miniupnpc)
* [libnatpmp](http://thebends.googlecode.com/svn/trunk/nat/pmp)
* upnp based on upnp implementation in [ktorrent](http://ktorrent.sourceforce.net)
* [pupnp](http://pupnp.sourceforge.net) (BSD License) latest update Apr 2012, first code 2005
* http://www.keymolen.com/ssdp.html C++ SSDP library, non-OSS
* https://code.google.com/p/upnpx/ objc/C++ upnp and ssdp lib, BSD License, based on pupnp, abandoned Nov 2012
* [reTurn C++ asio BSD STUN and TURN library](http://www.resiprocate.org/ReTurn_Overview)
* [nat-pmp and upnp implementation in maidsafe](https://code.google.com/p/maidsafe-dht/source/browse/trunk/src/maidsafe/nat-pmp/natpmpclient.h) 2009
