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
 void SetShutdownSendCallback   (Callback<int> cb) { m_shutdownSendCb = cb; }
 void SetShutdownRecvCallback   (Callback<int> cb) { m_shutdownRecvCb = cb; }
 void SetSendCallback           (Callback<int> cb) { m_sendCb = cb; }
 void SetSendToCallback         (Callback<int> cb) { m_sendToCb = cb; }
 void SetRecvCallback           (Callback< Ptr<Packet> > cb) { m_recvCb = cb; }
 void SetRecvFromCallback       (Callback< Ptr<Packet> > cb) { m_recvFromCb = cb; }
 void SetGetTxAvailableCallback (Callback<uint32_t> cb) { m_getTxAvailableCb = cb; }
 void SetGetRxAvailableCallback (Callback<uint32_t> cb) { m_getRxAvailableCb = cb; }

 void SetForwardUpCallback      (Callback<void> cb) { m_fordwardUpCb = cb; }
 void SetForwardUp6Callback     (Callback<void> cb) { m_forwardUp6Cb = cb; }
 void SetForwardIcmpCallback    (Callback<void> cb) { m_forwardIcmpCb = cb;}
 void SetForwardIcmp6Callback   (Callback<void> cb) { m_forwardIcmp6Cb = cb; }
 void SetDestroyCallback        (Callback<void> cb) { m_destroyCb = cb; }
 void SetDestroy6Callback       (Callback<void> cb) { m_destroy6Cb = cb; }
 void SetDoConnectCallback      (Callback<int> cb) { m_doConnectCb = cb; }

 void SetGetRxBufferSizeCallback    (Callback<uint32_t> cb) { m_getRxBufferSizeCb = cb; }
 void SetHasPendingDataCallback     (Callback<bool> cb) { m_hasPendingDataCb = cb; }
 void SetCloseAndNotifyCallback     (Callback<void> cb) { m_closeAndNotifyCb = cb; }
 void SetDeallocateEndPointCallback (Callback<void> cb) { m_deallocateEndPointCb = cb; }

 void SetCancelAllTimersCallback (Callback<void> cb) { m_cancelAllTimersCb = cb; }
 void SetSendRSTCallback         (Callback<void> cb) { m_sendRSTCb = cb; }
 void SetSendFINCallback         (Callback<void> cb) { m_sendFINCb = cb; }
 void SetSendFINACKCallback      (Callback<void> cb) { m_sendFINACKCb = cb; }

private:
 Callback<int> m_shutdownSendCb;
 Callback<int> m_shutdownRecvCb;
 Callback<int> m_sendCb;
 Callback<int> m_sendToCb;
 Callback< Ptr<Packet> > m_recvCb;
 Callback< Ptr<Packet> > m_recvFromCb;
 Callback<uint32_t> m_getTxAvailableCb;
 Callback<uint32_t> m_getRxAvailableCb;
 Callback<void> m_fordwardUpCb;
 Callback<void> m_forwardUp6Cb;
 Callback<void> m_forwardIcmpCb;
 Callback<void> m_forwardIcmp6Cb;
 Callback<void> m_destroyCb;
 Callback<void> m_destroy6Cb;
 Callback<int> m_doConnectCb;
 Callback<uint32_t> m_getRxBufferSizeCb;
 Callback<bool> m_hasPendingDataCb;
 Callback<void> m_closeAndNotifyCb;
 Callback<void> m_deallocateEndPointCb;
 Callback<void> m_cancelAllTimersCb;
 Callback<void> m_sendRSTCb;
 Callback<void> m_sendFINCb;
 Callback<void> m_sendFINACKCb;


public:
 int ShutdownSend (void) { return m_shutdownSendCb (); }
 int ShutdownRecv (void) { return m_shutdownRecvCb (); }

 int Send (Ptr<Packet> p, uint32_t flags) { return m_sendCb (); }
 int SendTo (Ptr<Packet> p, uint32_t flags, const Address &toAddress) { return m_sendToCb (); }

 Ptr<Packet> Recv    (uint32_t maxSize, uint32_t flags) { return m_recvCb (); }
 Ptr<Packet> RecvFrom(uint32_t maxSize, uint32_t flags,
                      Address &fromAddress) { return m_recvFromCb ();}
 uint32_t GetTxAvailable  (void) const { return m_getTxAvailableCb (); }
 uint32_t GetRxAvailable  (void) const { return m_getRxAvailableCb (); }

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

 int DoConnect (void) { return m_doConnectCb (); }
 uint32_t GetRxBufferSize (void) const { return m_getRxBufferSizeCb (); }
 bool HasPendingData (void) const { return m_hasPendingDataCb (); }
 void CloseAndNotify (void) { m_closeAndNotifyCb (); }
 void DeallocateEndPoint (void) { m_deallocateEndPointCb (); }

 void CancelAllTimers (void) { m_cancelAllTimersCb (); }
 void SendRST    (void) { m_sendRSTCb (); }
 void SendFIN    (void) { m_sendFINCb (); }
 void SendFINACK (void) { m_sendFINACKCb (); }
};

class Ns3TcpSocketImplTest : public TestCase, public Ns3TcpSocketImpl
{
public:
  Ns3TcpSocketImplTest (bool useIpv6, std::string name);

  int ShutdownSend (void);
  int ShutdownRecv (void);

  int Send (Ptr<Packet> p, uint32_t flags) ;
  int SendTo (Ptr<Packet> p, uint32_t flags, const Address &toAddress);

  Ptr<Packet> Recv    (uint32_t maxSize, uint32_t flags);
  Ptr<Packet> RecvFrom(uint32_t maxSize, uint32_t flags,
                       Address &fromAddress);

  virtual bool SetAllowBroadcast (bool allowBroadcast);
  virtual bool GetAllowBroadcast () const;

  uint32_t GetTxAvailable  (void) const;
  uint32_t GetRxAvailable  (void) const;

 protected:
  void ForwardUp (Ptr<Packet> packet, Ipv4Header header, uint16_t port,
                  Ptr<Ipv4Interface> incomingInterface);

  void ForwardUp6 (Ptr<Packet> packet, Ipv6Header header, uint16_t port,
                   Ptr<Ipv6Interface> incomingInterface);

  void ForwardIcmp (Ipv4Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                    uint8_t icmpCode, uint32_t icmpInfo);
  void ForwardIcmp6 (Ipv6Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                    uint8_t icmpCode, uint32_t icmpInfo);

  void Destroy (void);
  void Destroy6 (void);
  int DoConnect (void);
  uint32_t GetRxBufferSize (void) const;
  bool HasPendingData (void) const;
  void CloseAndNotify (void);
  void DeallocateEndPoint (void);

  void CancelAllTimers (void);
  void SendRST    (void);
  void SendFIN    (void);
  void SendFINACK (void);

private:
  virtual void SetSndBufSize (uint32_t size);
  virtual uint32_t GetSndBufSize (void) const;
  virtual void SetRcvBufSize (uint32_t size);
  virtual uint32_t GetRcvBufSize (void) const;
  virtual void SetSegSize (uint32_t size);
  virtual uint32_t GetSegSize (void) const;
  virtual void SetInitialSSThresh (uint32_t threshold);
  virtual uint32_t GetInitialSSThresh (void) const;
  virtual void SetInitialCwnd (uint32_t cwnd);
  virtual uint32_t GetInitialCwnd (void) const;
  virtual void SetConnTimeout (Time timeout);
  virtual Time GetConnTimeout (void) const;
  virtual void SetConnCount (uint32_t count);
  virtual uint32_t GetConnCount (void) const;
  virtual void SetDelAckTimeout (Time timeout);
  virtual Time GetDelAckTimeout (void) const;
  virtual void SetDelAckMaxCount (uint32_t count);
  virtual uint32_t GetDelAckMaxCount (void) const;
  virtual void SetTcpNoDelay (bool noDelay);
  virtual bool GetTcpNoDelay (void) const;
  virtual void SetPersistTimeout (Time timeout);
  virtual Time GetPersistTimeout (void) const;

protected:
  Ptr<Node> CreateInternetNode ();
  Ptr<Node> CreateInternetNode6 ();

  void SetupDefaultSim ();
  void SetupDefaultSim6 ();

  Ptr<SimpleNetDevice> AddSimpleNetDevice (Ptr<Node> node, const char* ipaddr,
                                           const char* netmask);
  Ptr<SimpleNetDevice> AddSimpleNetDevice6 (Ptr<Node> node, Ipv6Address ipaddr,
                                            Ipv6Prefix prefix);

private:
  virtual void DoRun (void);
  virtual void DoTeardown (void);
  bool m_useIpv6;
  uint32_t m_nodeId;

  const char* m_netmask;
  const char* m_ipaddr0;
  const int   m_ipv6Prefix;
  const char*  m_ipv6Addr;

  Ptr<Ns3TcpSocketImpl> m_socket;
};


Ns3TcpSocketImplTest::Ns3TcpSocketImplTest (bool useIpv6, std::string name)
  : TestCase (name),
    m_useIpv6 (useIpv6),
    m_netmask ("255.255.255.0"),
    m_ipaddr0 ("192.168.1.1"),
    m_ipv6Prefix (64),
    m_ipv6Addr ("2001:0100:f00d:cafe::1")
{

}

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

  m_nodeId = node->GetId ();

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

  m_nodeId = node->GetId();

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
  Ptr<Node> node0 = CreateInternetNode ();

  Ptr<SimpleNetDevice> dev0 = AddSimpleNetDevice (node0, m_ipaddr0, m_netmask);

  Ptr<SimpleChannel> channel = CreateObject<SimpleChannel> ();
  dev0->SetChannel (channel);

  Ptr<SocketFactory> sockFactory0 = node0->GetObject<TcpSocketFactory> ();

  Ptr<Socket> socket = sockFactory0->CreateSocket ();
  m_socket = DynamicCast<Ns3TcpSocketImpl> (socket);
}

void
Ns3TcpSocketImplTest::SetupDefaultSim6 (void)
{
  Ipv6Prefix prefix = Ipv6Prefix(m_ipv6Prefix);
  Ipv6Address ipaddr = Ipv6Address(m_ipv6Addr);

  Ptr<Node> node0 = CreateInternetNode6 ();
  Ptr<SimpleNetDevice> dev0 = AddSimpleNetDevice6 (node0, ipaddr, prefix);

  Ptr<SimpleChannel> channel = CreateObject<SimpleChannel> ();
  dev0->SetChannel (channel);

  Ptr<SocketFactory> sockFactory0 = node0->GetObject<TcpSocketFactory> ();

  Ptr<Socket> socket = sockFactory0->CreateSocket ();
  m_socket = DynamicCast<Ns3TcpSocketImpl> (socket);
}

void
Ns3TcpSocketImplTest::DoRun()
{
  if (m_useIpv6 == true)
    {
      SetupDefaultSim6 ();
    }
  else
    {
      SetupDefaultSim ();
    }

  NS_TEST_ASSERT_MSG_NE (m_socket, 0, "Cast from Socket to Ns3TcpSocketImpl failed");
  NS_TEST_ASSERT_MSG_EQ (m_socket->GetSocketType(), NS3_SOCK_STREAM,
                         "Different type of socket than expected");
  NS_TEST_ASSERT_MSG_EQ (m_socket->GetNode()->GetId(), m_nodeId,
                         "Different NodeID returned. Swapping of nodes?");

  // TODO in ns3TcpSocketImpl
  //int ret;
  //Address addr;
  //ret = m_socket->GetSockName(&addr);



}

static class Ns3TcpSocketImplTestSuite : public TestSuite
{
public:
  Ns3TcpSocketImplTestSuite ()
    : TestSuite ("ns3tcpsocket", UNIT)
  {
    AddTestCase (new Ns3TcpSocketImplTest (false, "noipv6"), TestCase::QUICK);
    AddTestCase (new Ns3TcpSocketImplTest (true,  "ipv6"),   TestCase::QUICK);
  }

} g_n3TcpSocketImplTestSuite;
