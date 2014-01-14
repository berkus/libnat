/* Copyright (c) 2009 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "nat/maidsafe_utils.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_array.hpp>
#include <boost/thread/mutex.hpp>
#include <ctype.h>
#include <algorithm>
#include <limits>
#include <string>
#include "nat/maidsafe_network_interface.h"

namespace base {

// POSIX and APPLE Socket implementation
#if defined(MAIDSAFE_POSIX) || defined (MAIDSAFE_APPLE)
// || defined (__MINGW__)
static boost::uint32_t SockAddrToUint32(struct sockaddr * a) {
  return ((a)&&(a->sa_family == AF_INET)) ?
         ntohl(((struct sockaddr_in *)a)->sin_addr.s_addr) : 0;
}
#endif

void GetNetInterfaces(std::vector<struct DeviceStruct> *alldevices) {
  alldevices->clear();
  boost::system::error_code ec;
  std::vector<NetworkInterface> net_interfaces;
  net_interfaces = NetworkInterface::LocalList(ec);
  if (ec)
    return;
  for (std::vector<NetworkInterface>::iterator it = net_interfaces.begin();
       it != net_interfaces.end(); ++it) {
    DeviceStruct device;
    device.device_interface = std::string(it->name);
    device.ip_address = it->destination;
    alldevices->push_back(device);
  }
}

bool GetLocalAddress(boost::asio::ip::address *local_address) {
  *local_address = NetworkInterface::LocalAddress();
  if (NetworkInterface::IsAny(*local_address))
    return false;
  return true;
}

std::vector<std::string> GetLocalAddresses() {
  // get all network interfaces
  std::vector<std::string> addresses;
  std::vector<struct DeviceStruct> alldevices;
  GetNetInterfaces(&alldevices);
  for (std::vector<struct DeviceStruct>::iterator it = alldevices.begin();
       it != alldevices.end(); ++it) {
    if (!NetworkInterface::IsLoopback(it->ip_address) &&
        !NetworkInterface::IsMulticast(it->ip_address) &&
        !NetworkInterface::IsAny(it->ip_address)) {
      addresses.push_back(it->ip_address.to_string());
    }
  }
  return addresses;
}

}  // namespace base
