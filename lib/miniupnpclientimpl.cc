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

#include "nat/miniupnpclientimpl.h"
#include <boost/bind.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include "nat/maidsafe_utils.h"
#include "libupnp/miniwget.h"
#include "libupnp/miniupnpc.h"
#include "libupnp/upnpcommands.h"

#define DEBUG

namespace upnp {

UpnpIgdClientImpl::UpnpIgdClientImpl()
  : is_initialised_(false), has_services_(false),
    upnp_urls_(), igd_data_(),
    port_mappings_(), timer_() {}

UpnpIgdClientImpl::~UpnpIgdClientImpl() {
  timer_.CancelAll();
  DeleteAllPortMappings();
}

bool UpnpIgdClientImpl::IsAsync() {
  return false;
}

bool UpnpIgdClientImpl::HasServices() {
  return has_services_;
}

bool UpnpIgdClientImpl::InitControlPoint() {
  if (is_initialised_)
    return false;

  DiscoverDevices();

  is_initialised_ = true;

  timer_.AddCallLater(kLeaseDuration * 1000,
                      boost::bind(&UpnpIgdClientImpl::RefreshCallback, this));

  return is_initialised_;  // && has_services_;
}

bool UpnpIgdClientImpl::GetPortMappings(std::list<PortMappingExt>& out_mapping)
{
  int ret;

  out_mapping.clear();

// get the state var instead
// <relatedStateVariable>PortMappingNumberOfEntries</relatedStateVariable>
// @todo Parse SCPD and get actual supported command and var names from there...

  int i = 0;
  while (true)
  {
    char extPort[6] = {0};
    char intClient[16] = {0};
    char intPort[6] = {0};
    char protocol[4] = {0};
    char desc[80] = {0};
    char enabled[4] = {0};
    char rHost[64] = {0};
    char duration[16] = {0};

    char index[16] = {0};
    snprintf(index, 16, "%u", i);
    ret = UPNP_GetGenericPortMappingEntry(upnp_urls_.controlURL,
                                          igd_data_.first.servicetype,
                                          index,
                                          extPort,
                                          intClient,
                                          intPort,
                                          protocol,
                                          desc,
                                          enabled,
                                          rHost,
                                          duration);

    if (ret == 501) {
      break;
    }
    // Compal Broadband Networks, Inc/Linux/2.6.39.3 UPnP/1.1 MiniUPnPd/1.7 spits this:
    // 713 - SpecifiedArrayIndexInvalid
    if (ret == 713) {
      break;
    }
    assert(ret == UPNPCOMMAND_SUCCESS);

    PortMappingExt pm(boost::lexical_cast<int>(intPort),
                   boost::lexical_cast<int>(extPort),
                   (std::string(protocol) == "TCP" ? kTcp : kUdp));
    pm.enabled = (std::string(enabled) == "1" ? true : false);
    pm.internal_host = intClient;
    pm.external_host = rHost;
    pm.duration = boost::lexical_cast<int>(duration);
    pm.description = desc;

    out_mapping.emplace_back(pm);

    ++i;
  }

  return true;
}

bool UpnpIgdClientImpl::AddPortMapping(const PortMapping &mapping) {
  PortMapping *pm;
  std::list<PortMapping>::iterator it;
  if (PortMappingExists(mapping.external_port, mapping.protocol, it)) {
    pm = &(*it);
  } else {
    port_mappings_.push_back(mapping);
    pm = &port_mappings_.back();
  }

  std::string extPort = boost::lexical_cast<std::string>(mapping.external_port);
  std::string intPort = boost::lexical_cast<std::string>(mapping.internal_port);
  std::string proto = (mapping.protocol == kTcp ? "TCP" : "UDP");

#ifdef DEBUG
  /* if (pm->enabled) {
    printf("UPnP port mapping already exists: %s %s\n",
           proto.c_str(), ext_port.c_str());
  } */
#endif

  if (has_services_) {
    boost::asio::ip::address ip_addr;
    base::GetLocalAddress(&ip_addr);

    int res = UPNP_AddPortMapping(upnp_urls_.controlURL,
                                  igd_data_.first.servicetype,
                                  extPort.c_str(), intPort.c_str(),
                                  ip_addr.to_string().c_str(),
                                  kClientName,
                                  proto.c_str(),
                                  NULL,
                                  boost::lexical_cast<std::string>(kad::kRefreshTime).c_str());

    if (res == UPNPCOMMAND_SUCCESS) {
      pm->enabled = true;
    } else {
      pm->enabled = false;
#ifdef DEBUG
      printf("Error adding UPnP port mapping (%s %s): %d\n",
             proto.c_str(),
             extPort.c_str(),
             res);
#endif
    }

    return res == UPNPCOMMAND_SUCCESS;
  } else {
    return false;
  }
}

bool UpnpIgdClientImpl::PortMappingExists(
       const int &port, const ProtocolType &protocol,
       std::list<PortMapping>::iterator &it) {
  for (it = port_mappings_.begin(); it != port_mappings_.end(); ++it) {
    if ((*it).external_port == port &&
        (*it).protocol == protocol)
      return true;
  }
  return false;
}

bool UpnpIgdClientImpl::DeletePortMapping(const int &port,
                                          const ProtocolType &protocol) {
  std::list<PortMapping>::iterator it_pm;
  if (PortMappingExists(port, protocol, it_pm)) {
    bool ok = true;
    if ((*it_pm).enabled) {
      std::string ext_port = boost::lexical_cast<std::string>(port);
      std::string proto = (protocol == kTcp ? "TCP" : "UDP");

      int res = UPNP_DeletePortMapping(upnp_urls_.controlURL,
                                       igd_data_.first.servicetype,
                                       ext_port.c_str(),
                                       proto.c_str(),
                                       NULL);

#ifdef DEBUG
      if (res != UPNPCOMMAND_SUCCESS) {
        printf("Error deleting UPnP port mapping (%s %d): %d\n", proto.c_str(),
            port, res);
      }
#endif

      ok = (res == UPNPCOMMAND_SUCCESS);
    }
    port_mappings_.erase(it_pm);
    return ok;
  }
  return false;
}

bool UpnpIgdClientImpl::DeleteAllPortMappings() {
  bool result = true;
  while (!port_mappings_.empty()) {
    result &= DeletePortMapping(port_mappings_.front().external_port,
                                port_mappings_.front().protocol);
  }
  return result;
}

std::string UpnpIgdClientImpl::GetExternalIpAddress() {
  char ip[16];
  ip[0] = '\0';
  if (has_services_) {
    int res = UPNP_GetExternalIPAddress(upnp_urls_.controlURL,
                                        igd_data_.first.servicetype, ip);
    if (res == UPNPCOMMAND_SUCCESS) {
      return std::string(ip);
    }
  }
  return "";
}

void UpnpIgdClientImpl::DiscoverDevices() {
  struct UPNPDev* devlist = upnpDiscover(kSearchTime * 1000, NULL, NULL, 0, /*IPv6:*/0, NULL);
  int res = UPNP_GetValidIGD(devlist, &upnp_urls_, &igd_data_, NULL, 0);
  freeUPNPDevlist(devlist);

  has_services_ = (res == 1);
}

bool IsUpnpIgdConnected(const UPNPUrls &urls, const IGDdatas &data) {
  char status[64];
  unsigned int uptime;
  status[0] = '\0';
  UPNP_GetStatusInfo(urls.controlURL, data.first.servicetype,
                     status, &uptime, NULL);
  return (0 == strcmp("Connected", status));
}

void UpnpIgdClientImpl::RefreshCallback() {
#ifdef DEBUG
  if (port_mappings_.size() > 0)
    printf("Refreshing UPnP port mappings...\n");
#endif

  if (!has_services_ || !IsUpnpIgdConnected(upnp_urls_, igd_data_)) {
    DiscoverDevices();
  }

  for (std::list<PortMapping>::iterator it = port_mappings_.begin();
       it != port_mappings_.end(); ++it) {
    if (has_services_) {
      AddPortMapping(*it);
    } else {
      (*it).enabled = false;
    }
  }

  timer_.AddCallLater(kLeaseDuration * 1000,
                      boost::bind(&UpnpIgdClientImpl::RefreshCallback, this));
}

}  // namespace upnp
