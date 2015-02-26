/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Natale Patriciello <natale.patriciello@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "ns3/test.h"
#include "ns3/socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/ns3-tcp-socket-impl.h"
#include "ns3/simulator.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/config.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/ipv6-list-routing.h"
#include "ns3/node.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

#include "ns3/ipv4-end-point.h"
#include "ns3/arp-l3-protocol.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/icmpv4-l4-protocol.h"
#include "ns3/icmpv6-l4-protocol.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/tcp-l4-protocol.h"

#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Ns3TcpSocketImplTest");

class Ns3TcpSocketImplDerived : public Ns3TcpSocketImpl
{
public:
 void SetShutdownSendCallback   (Callback<void> cb) { m_shutdownSendCb = cb; }
 void SetShutdownRecvCallback   (Callback<void> cb) { m_shutdownRecvCb = cb; }
 void SetSendCallback           (Callback<void> cb) { m_sendCb = cb; }
 void SetSendToCallback         (Callback<void> cb) { m_sendToCb = cb; }
 void SetRecvCallback           (Callback<void> cb) { m_recvCb = cb; }
 void SetRecvFromCallback       (Callback<void> cb) { m_recvFromCb = cb; }
 void SetGetTxAvailableCallback (Callback<void> cb) { m_getTxAvailableCb = cb; }
 void SetGetRxAvailableCallback (Callback<void> cb) { m_getRxAvailableCb = cb; }

 void SetForwardUpCallback      (Callback<void> cb) { m_fordwardUpCb = cb; }
 void SetForwardUp6Callback     (Callback<void> cb) { m_forwardUp6Cb = cb; }
 void SetForwardIcmpCallback    (Callback<void> cb) { m_forwardIcmpCb = cb;}
 void SetForwardIcmp6Callback   (Callback<void> cb) { m_forwardIcmp6Cb = cb; }
 void SetDestroyCallback        (Callback<void> cb) { m_destroyCb = cb; }
 void SetDestroy6Callback       (Callback<void> cb) { m_destroy6Cb = cb; }
 void SetDoConnectCallback      (Callback<void> cb) { m_doConnectCb = cb; }

 void SetGetRxBufferSizeCallback    (Callback<void> cb) { m_getRxBufferSizeCb = cb; }
 void SetHasPendingDataCallback     (Callback<void> cb) { m_hasPendingDataCb = cb; }
 void SetCloseAndNotifyCallback     (Callback<void> cb) { m_closeAndNotifyCb = cb; }
 void SetDeallocateEndPointCallback (Callback<void> cb) { m_deallocateEndPointCb = cb; }

 void SetCancelAllTimersCallback (Callback<void> cb) { m_cancelAllTimersCb = cb; }
 void SetSendRSTCallback         (Callback<void> cb) { m_sendRSTCb = cb; }
 void SetSendFINCallback         (Callback<void> cb) { m_sendFINCb = cb; }
 void SetSendFINACKCallback      (Callback<void> cb) { m_sendFINACKCb = cb; }

private:
 Callback<void> m_shutdownSendCb;
 Callback<void> m_shutdownRecvCb;
 Callback<void> m_sendCb;
 Callback<void> m_sendToCb;
 Callback<void> m_recvCb;
 Callback<void> m_recvFromCb;
 Callback<void> m_getTxAvailableCb;
 Callback<void> m_getRxAvailableCb;
 Callback<void> m_fordwardUpCb;
 Callback<void> m_forwardUp6Cb;
 Callback<void> m_forwardIcmpCb;
 Callback<void> m_forwardIcmp6Cb;
 Callback<void> m_destroyCb;
 Callback<void> m_destroy6Cb;
 Callback<void> m_doConnectCb;
 Callback<void> m_getRxBufferSizeCb;
 Callback<void> m_hasPendingDataCb;
 Callback<void> m_closeAndNotifyCb;
 Callback<void> m_deallocateEndPointCb;
 Callback<void> m_cancelAllTimersCb;
 Callback<void> m_sendRSTCb;
 Callback<void> m_sendFINCb;
 Callback<void> m_sendFINACKCb;


public:
 int ShutdownSend (void) { m_shutdownSendCb (); }
 int ShutdownRecv (void) { m_shutdownRecvCb (); }

 int Send (Ptr<Packet> p, uint32_t flags) { m_sendCb (); }
 int SendTo (Ptr<Packet> p, uint32_t flags, const Address &toAddress) { m_sendToCb (); }

 Ptr<Packet> Recv    (uint32_t maxSize, uint32_t flags) { m_recvCb (); }
 Ptr<Packet> RecvFrom(uint32_t maxSize, uint32_t flags,
                      Address &fromAddress) { m_recvFromCb ();}
 uint32_t GetTxAvailable  (void) const { m_getTxAvailableCb (); }
 uint32_t GetRxAvailable  (void) const { m_getRxAvailableCb (); }

protected:
 void ForwardUp (Ptr<Packet> packet, Ipv4Header header, uint16_t port,
                 Ptr<Ipv4Interface> incomingInterface) { m_fordwardUpCb (); }

 void ForwardUp6 (Ptr<Packet> packet, Ipv6Header header, uint16_t port,
                  Ptr<Ipv6Interface> incomingInterface) { m_forwardUp6Cb (); }

 void ForwardIcmp (Ipv4Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                   uint8_t icmpCode, uint32_t icmpInfo) { m_forwardIcmpCb (); }
 void ForwardIcmp6 (Ipv6Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                   uint8_t icmpCode, uint32_t icmpInfo) { m_forwardIcmp6Cb (); }

 void Destroy (void) { m_destroyCb (); }
 void Destroy6 (void) { m_destroy6Cb (); }

 int DoConnect (void) { m_doConnectCb (); }
 uint32_t GetRxBufferSize (void) const { m_getRxBufferSizeCb (); }
 bool HasPendingData (void) const { m_hasPendingDataCb (); }
 void CloseAndNotify (void) { m_closeAndNotifyCb (); }
 void DeallocateEndPoint (void) { m_deallocateEndPointCb (); }

 void CancelAllTimers (void) { m_cancelAllTimersCb (); }
 void SendRST    (void) { m_sendRSTCb (); }
 void SendFIN    (void) { m_sendFINCb (); }
 void SendFINACK (void) { m_sendFINACKCb (); }
};

class Ns3TcpSocketImplTest : public TestCase
{
public:
  Ns3TcpSocketImplTest ();

private:
  virtual void DoRun (void);
  virtual void DoTeardown (void);
};


Ptr<Node>
Ns3TcpSocketImplTest::CreateInternetNode ()
{
  Ptr<Node> node = CreateObject<Node> ();
  //ARP
  Ptr<ArpL3Protocol> arp = CreateObject<ArpL3Protocol> ();
  node->AggregateObject (arp);
  //IPV4
  Ptr<Ipv4L3Protocol> ipv4 = CreateObject<Ipv4L3Protocol> ();
  //Routing for Ipv4
  Ptr<Ipv4ListRouting> ipv4Routing = CreateObject<Ipv4ListRouting> ();
  ipv4->SetRoutingProtocol (ipv4Routing);
  Ptr<Ipv4StaticRouting> ipv4staticRouting = CreateObject<Ipv4StaticRouting> ();
  ipv4Routing->AddRoutingProtocol (ipv4staticRouting, 0);
  node->AggregateObject (ipv4);
  //ICMP
  Ptr<Icmpv4L4Protocol> icmp = CreateObject<Icmpv4L4Protocol> ();
  node->AggregateObject (icmp);
  //UDP
  Ptr<UdpL4Protocol> udp = CreateObject<UdpL4Protocol> ();
  node->AggregateObject (udp);
  //TCP
  Ptr<TcpL4Protocol> tcp = CreateObject<TcpL4Protocol> ();
  node->AggregateObject (tcp);
  return node;
}

Ptr<Node>
Ns3TcpSocketImplTest::CreateInternetNode6 ()
{
  Ptr<Node> node = CreateObject<Node> ();
  //IPV6
  Ptr<Ipv6L3Protocol> ipv6 = CreateObject<Ipv6L3Protocol> ();
  //Routing for Ipv6
  Ptr<Ipv6ListRouting> ipv6Routing = CreateObject<Ipv6ListRouting> ();
  ipv6->SetRoutingProtocol (ipv6Routing);
  Ptr<Ipv6StaticRouting> ipv6staticRouting = CreateObject<Ipv6StaticRouting> ();
  ipv6Routing->AddRoutingProtocol (ipv6staticRouting, 0);
  node->AggregateObject (ipv6);
  //ICMP
  Ptr<Icmpv6L4Protocol> icmp = CreateObject<Icmpv6L4Protocol> ();
  node->AggregateObject (icmp);
  //Ipv6 Extensions
  ipv6->RegisterExtensions ();
  ipv6->RegisterOptions ();
  //UDP
  Ptr<UdpL4Protocol> udp = CreateObject<UdpL4Protocol> ();
  node->AggregateObject (udp);
  //TCP
  Ptr<TcpL4Protocol> tcp = CreateObject<TcpL4Protocol> ();
  node->AggregateObject (tcp);
  return node;
}

Ptr<SimpleNetDevice>
Ns3TcpSocketImplTest::AddSimpleNetDevice6 (Ptr<Node> node, Ipv6Address ipaddr,
                                           Ipv6Prefix prefix)
{
  Ptr<SimpleNetDevice> dev = CreateObject<SimpleNetDevice> ();
  dev->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
  node->AddDevice (dev);
  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
  uint32_t ndid = ipv6->AddInterface (dev);
  Ipv6InterfaceAddress ipv6Addr = Ipv6InterfaceAddress (ipaddr, prefix);
  ipv6->AddAddress (ndid, ipv6Addr);
  ipv6->SetUp (ndid);
  return dev;
}

Ptr<SimpleNetDevice>
Ns3TcpSocketImplTest::AddSimpleNetDevice (Ptr<Node> node, const char* ipaddr,
                                          const char* netmask)
{
  Ptr<SimpleNetDevice> dev = CreateObject<SimpleNetDevice> ();
  dev->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
  node->AddDevice (dev);
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
  uint32_t ndid = ipv4->AddInterface (dev);
  Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address (ipaddr),
                                                        Ipv4Mask (netmask));
  ipv4->AddAddress (ndid, ipv4Addr);
  ipv4->SetUp (ndid);
  return dev;
}

void
Ns3TcpSocketImplTest::SetupDefaultSim (void)
{
  const char* netmask = "255.255.255.0";
  const char* ipaddr0 = "192.168.1.1";

  Ptr<Node> node0 = CreateInternetNode ();

  Ptr<SimpleNetDevice> dev0 = AddSimpleNetDevice (node0, ipaddr0, netmask);

  Ptr<SimpleChannel> channel = CreateObject<SimpleChannel> ();
  dev0->SetChannel (channel);

  Ptr<SocketFactory> sockFactory0 = node0->GetObject<TcpSocketFactory> ();
  Ptr<SocketFactory> sockFactory1 = node1->GetObject<TcpSocketFactory> ();

  Ptr<Socket> server = sockFactory0->CreateSocket ();
}

void
Ns3TcpSocketImplTest::SetupDefaultSim6 (void)
{
  Ipv6Prefix prefix = Ipv6Prefix(64);
  Ipv6Address ipaddr0 = Ipv6Address("2001:0100:f00d:cafe::1");
  Ptr<Node> node0 = CreateInternetNode6 ();
  Ptr<SimpleNetDevice> dev0 = AddSimpleNetDevice6 (node0, ipaddr0, prefix);

  Ptr<SimpleChannel> channel = CreateObject<SimpleChannel> ();
  dev0->SetChannel (channel);

  Ptr<SocketFactory> sockFactory0 = node0->GetObject<TcpSocketFactory> ();
  Ptr<SocketFactory> sockFactory1 = node1->GetObject<TcpSocketFactory> ();

  Ptr<Socket> server = sockFactory0->CreateSocket ();
}

static class Ns3TcpSocketImplTestSuite : public TestSuite
{
public:
  Ns3TcpSocketImplTestSuite ()
    : TestSuite ("ns3tcpsocket", UNIT)
  {
  }

} g_n3TcpSocketImplTestSuite;
