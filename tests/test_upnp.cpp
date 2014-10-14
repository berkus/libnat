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
#define BOOST_TEST_MODULE Test_upnp
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <boost/bind.hpp>
#include "nat/maidsafe-dht_config.h"
#include "nat/upnpclient.h"

// #include "protocol.h"
const int default_port = 9660; // Define port in-place instead of sss dependency

using namespace std;

// Test depends on external UPnP device, but doesn't fail if none found

class UpnpTest {
 public:
  UpnpTest() : num_total_mappings(3), num_curr_mappings(0) {}
  void OnNewMapping(const int &port, const upnp::ProtocolType &protocol);
  void OnLostMapping(const int &port, const upnp::ProtocolType &protocol);
  void OnFailedMapping(const int &port, const upnp::ProtocolType &protocol);

  int num_total_mappings;
  int num_curr_mappings;
};

void UpnpTest::OnNewMapping(const int &port,
                            const upnp::ProtocolType &protocol) {
  num_curr_mappings++;
  printf("New port mapping: %s %d\n",
         protocol == upnp::kUdp ? "UDP" : "TCP", port);
}

void UpnpTest::OnLostMapping(const int &port,
                             const upnp::ProtocolType &protocol) {
  num_curr_mappings--;
  printf("Lost port mapping: %s %d\n",
         protocol == upnp::kUdp ? "UDP" : "TCP", port);
}

void UpnpTest::OnFailedMapping(const int &port,
                               const upnp::ProtocolType &protocol) {
  printf("Failed port mapping: %s %d\n",
         protocol == upnp::kUdp ? "UDP" : "TCP", port);
}

BOOST_AUTO_TEST_CASE(UPNP_TcpPortMappingTest)
{
    upnp::UpnpIgdClient upnp;
    UpnpTest test;

    printf("Initialising UPnP...\n");

    BOOST_CHECK(upnp.InitControlPoint());

    if (upnp.IsAsync()) {
      upnp.SetNewMappingCallback(
        boost::bind(&UpnpTest::OnNewMapping, &test, _1, _2));
      upnp.SetLostMappingCallback(
        boost::bind(&UpnpTest::OnLostMapping, &test, _1, _2));
      upnp.SetFailedMappingCallback(
        boost::bind(&UpnpTest::OnFailedMapping, &test, _1, _2));
    }

    // this_thread::sleep_for(chrono::seconds(2));

    std::list<upnp::PortMappingExt> out_mapping;
    upnp.GetPortMappings(out_mapping);

    for (auto m : out_mapping)
    {
      cout << "UPNP mapping found: int host " << m.internal_host << ", int port " << m.internal_port
        << " to ext host " << m.external_host << ", ext port " << m.external_port << endl
        << " protocol " << m.protocol << ", enabled " << m.enabled << ", duration " << m.duration
        << ", desc " << m.description << endl;
    }

    boost::int32_t start_port(default_port);

    bool all_added = true;
    for (int i = 0; i < test.num_total_mappings; ++i) {
      all_added &= upnp.AddPortMapping(start_port + i, upnp::kTcp);
    }

    if (upnp.IsAsync()) {
      printf("Waiting...\n");
      this_thread::sleep_for(chrono::seconds(3));
    }

    if (upnp.HasServices()) {
      printf("External IP: %s\n", upnp.GetExternalIpAddress().c_str());
      BOOST_CHECK(all_added);
      if (upnp.IsAsync()) {
        BOOST_CHECK(test.num_curr_mappings == test.num_total_mappings);
      }
      printf("All UPnP mappings successful.\n");
    } else {
      printf("Sorry, no port mappings via UPnP possible.\n");
    }
    BOOST_CHECK(upnp.DeletePortMapping(start_port + test.num_total_mappings - 1, upnp::kTcp));
    BOOST_CHECK(upnp.DeleteAllPortMappings());
}

BOOST_AUTO_TEST_CASE(UPNP_UdpPortMappingTest)
{
    upnp::UpnpIgdClient upnp;
    UpnpTest test;

    printf("Initialising UPnP...\n");

    BOOST_CHECK(upnp.InitControlPoint());

    if (upnp.IsAsync()) {
      upnp.SetNewMappingCallback(
        boost::bind(&UpnpTest::OnNewMapping, &test, _1, _2));
      upnp.SetLostMappingCallback(
        boost::bind(&UpnpTest::OnLostMapping, &test, _1, _2));
      upnp.SetFailedMappingCallback(
        boost::bind(&UpnpTest::OnFailedMapping, &test, _1, _2));
    }

    boost::int32_t start_port(default_port);

    bool all_added = true;
    for (int i = 0; i < test.num_total_mappings; ++i) {
      all_added &= upnp.AddPortMapping(start_port + i, upnp::kUdp);
    }

    if (upnp.IsAsync()) {
      printf("Waiting...\n");
      this_thread::sleep_for(chrono::seconds(3));
    }

    if (upnp.HasServices()) {
      printf("External IP: %s\n", upnp.GetExternalIpAddress().c_str());
      BOOST_CHECK(all_added);
      if (upnp.IsAsync()) {
        BOOST_CHECK(test.num_curr_mappings == test.num_total_mappings);
      }
      printf("All UPnP mappings successful.\n");
    } else {
      printf("Sorry, no port mappings via UPnP possible.\n");
    }
    BOOST_CHECK(upnp.DeletePortMapping(start_port + test.num_total_mappings - 1, upnp::kUdp));
    BOOST_CHECK(upnp.DeleteAllPortMappings());
}
