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
#include "ns3/object-vector.h"

#include <string>

#define protected public
#define private public

#include "ns3/ns3-tcp-socket-impl.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Ns3TcpSocketImplTest");

#define SAFELY_CALL_CB(name)   \
        do                     \
          {                    \
            if (! name.IsNull())\
              {                \
                name ();       \
              }                \
          } while (false);

#define SAFELY_CALL_AND_RETURN_CB(name, ret)   \
        do                     \
          {                    \
            if (! name.IsNull())\
              {                \
                return name ();\
              }                \
            return ret;        \
          } while (false)

class Ns3TcpSocketImplDerived : public Ns3TcpSocketImpl
{
public:
 static TypeId GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::Ns3TcpSocketImplDerived")
     .SetParent<Ns3TcpSocketImpl> ()
     .AddConstructor <Ns3TcpSocketImplDerived> ()
   ;
   return tid;
 }

 Ns3TcpSocketImplDerived () : Ns3TcpSocketImpl () { }

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
 int ShutdownSend (void) { SAFELY_CALL_AND_RETURN_CB (m_shutdownSendCb, 0); }
 int ShutdownRecv (void) { SAFELY_CALL_AND_RETURN_CB (m_shutdownRecvCb, 0); }

 int
 Send (Ptr<Packet> p, uint32_t flags)
 {
   SAFELY_CALL_AND_RETURN_CB (m_sendCb, 0);
 }

 int
 SendTo (Ptr<Packet> p, uint32_t flags, const Address &toAddress)
 {
   SAFELY_CALL_AND_RETURN_CB (m_sendToCb, 0);
 }

 Ptr<Packet>
 Recv (uint32_t maxSize, uint32_t flags)
 {
   SAFELY_CALL_AND_RETURN_CB (m_recvCb, 0);
 }

 Ptr<Packet>
 RecvFrom (uint32_t maxSize, uint32_t flags, Address &fromAddress)
 {
   SAFELY_CALL_AND_RETURN_CB (m_recvFromCb, 0);
 }

 uint32_t
 GetTxAvailable (void) const
 {
   SAFELY_CALL_AND_RETURN_CB (m_getTxAvailableCb, 0);
 }

 uint32_t
 GetRxAvailable (void) const
 {
   SAFELY_CALL_AND_RETURN_CB (m_getRxAvailableCb, 0);
 }

 bool
 SetAllowBroadcast (bool allowBroadcast)
 {
   return true;
 }

 bool
 GetAllowBroadcast() const
 {
   return false;
 }

protected:
 void ForwardUp (Ptr<Packet> packet, Ipv4Header header, uint16_t port,
                 Ptr<Ipv4Interface> incomingInterface)
 {
   SAFELY_CALL_CB (m_fordwardUpCb);
 }

 void ForwardUp6 (Ptr<Packet> packet, Ipv6Header header, uint16_t port,
                  Ptr<Ipv6Interface> incomingInterface)
 {
   SAFELY_CALL_CB (m_forwardUp6Cb);
 }

 void ForwardIcmp (Ipv4Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                   uint8_t icmpCode, uint32_t icmpInfo)
 {
   SAFELY_CALL_CB (m_forwardIcmpCb);
 }

 void ForwardIcmp6 (Ipv6Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                   uint8_t icmpCode, uint32_t icmpInfo)
 {
   SAFELY_CALL_CB (m_forwardIcmp6Cb);
 }

 void Destroy (void)
 {
   SAFELY_CALL_CB (m_destroyCb);
 }

 void Destroy6 (void)
 {
   SAFELY_CALL_CB (m_destroy6Cb);
 }

 int DoConnect (void)
 {
   SAFELY_CALL_AND_RETURN_CB (m_doConnectCb, 0);
 }

 uint32_t GetRxBufferSize (void) const
 {
   SAFELY_CALL_AND_RETURN_CB (m_getRxBufferSizeCb, 0);
 }

 bool HasPendingData (void) const
 {
   SAFELY_CALL_AND_RETURN_CB (m_hasPendingDataCb, false);
 }

 void CloseAndNotify (void)
 {
   SAFELY_CALL_CB (m_closeAndNotifyCb);
 }

 void DeallocateEndPoint (void)
 {
   SAFELY_CALL_CB (m_deallocateEndPointCb);
 }

 void CancelAllTimers (void)
 {
   SAFELY_CALL_CB (m_cancelAllTimersCb);
 }

 void SendRST    (void)
 {
   SAFELY_CALL_CB (m_sendRSTCb);
 }

 void SendFIN    (void)
 {
   SAFELY_CALL_CB (m_sendFINCb);
 }

 void SendFINACK (void)
 {
   SAFELY_CALL_CB (m_sendFINACKCb);
 }

private:
 virtual void SetSndBufSize (uint32_t size) {}
 virtual uint32_t GetSndBufSize (void) const {return 0;}
 virtual void SetRcvBufSize (uint32_t size) {}
 virtual uint32_t GetRcvBufSize (void) const {return 0;}
 virtual void SetSegSize (uint32_t size) {}
 virtual uint32_t GetSegSize (void) const {return 0;}
 virtual void SetInitialSSThresh (uint32_t threshold) {}
 virtual uint32_t GetInitialSSThresh (void) const {return 0;}
 virtual void SetInitialCwnd (uint32_t cwnd) {}
 virtual uint32_t GetInitialCwnd (void) const {return 0;}
 virtual void SetConnTimeout (Time timeout) {}
 virtual Time GetConnTimeout (void) const {return Time ();}
 virtual void SetConnCount (uint32_t count) {}
 virtual uint32_t GetConnCount (void) const {return 0;}
 virtual void SetDelAckTimeout (Time timeout){}
 virtual Time GetDelAckTimeout (void) const {return Time();}
 virtual void SetDelAckMaxCount (uint32_t count) {}
 virtual uint32_t GetDelAckMaxCount (void) const {return 0;}
 virtual void SetTcpNoDelay (bool noDelay) {}
 virtual bool GetTcpNoDelay (void) const {return false;}
 virtual void SetPersistTimeout (Time timeout) {}
 virtual Time GetPersistTimeout (void) const {return Time ();}
};

class Ns3TcpSocketImplTest : public TestCase
{
public:
  Ns3TcpSocketImplTest (bool useIpv6, std::string name);

  static Ptr<Node> CreateInternetNode ();
  static Ptr<Node> CreateInternetNode6 ();
  static Ptr<SimpleNetDevice> AddSimpleNetDevice (Ptr<Node> node, const char* ipaddr,
                                                  const char* netmask);
  static Ptr<SimpleNetDevice> AddSimpleNetDevice6 (Ptr<Node> node, Ipv6Address ipaddr,
                                                  Ipv6Prefix prefix);
protected:

  void SetupDefaultSim ();
  void SetupDefaultSim6();

  void CheckBind ();
  void CheckConnect ();

  void ForwardUpCb (void);
  void ForwardIcmpCb (void);
  void DestroyCb (void);

protected:
  bool m_forwardUpCbInvoked;
  bool m_forwardIcmpCbInvoked;
  bool m_destroyCbInvoked;

private:
  virtual void DoRun (void);

  bool     m_useIpv6;

  const char* m_netmask;
  const char* m_ipaddr0;
  const int   m_ipv6Prefix;
  const char* m_ipv6Addr;

  Ptr<Node> m_node;
};


Ns3TcpSocketImplTest::Ns3TcpSocketImplTest (bool useIpv6, std::string name)
  : TestCase (name),
    m_forwardUpCbInvoked (false),
    m_forwardIcmpCbInvoked (false),
    m_destroyCbInvoked (false),
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
Ns3TcpSocketImplTest::SetupDefaultSim ()
{
  m_node = CreateInternetNode ();
  Ptr<SimpleNetDevice> dev0 = AddSimpleNetDevice (m_node, m_ipaddr0, m_netmask);

  Ptr<SimpleChannel> channel = CreateObject<SimpleChannel> ();
  dev0->SetChannel (channel);
}

void
Ns3TcpSocketImplTest::SetupDefaultSim6 (void)
{
  Ipv6Prefix prefix = Ipv6Prefix(m_ipv6Prefix);
  Ipv6Address ipaddr = Ipv6Address(m_ipv6Addr);

  m_node = CreateInternetNode6 ();
  Ptr<SimpleNetDevice> dev0 = AddSimpleNetDevice6 (m_node, ipaddr, prefix);

  Ptr<SimpleChannel> channel = CreateObject<SimpleChannel> ();
  dev0->SetChannel (channel);
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

  Ptr<TcpL4Protocol> tcp = m_node->GetObject<TcpL4Protocol> ();
  Ptr<Socket> s = tcp->CreateSocket (Ns3TcpSocketImplDerived::GetTypeId());

  Ptr<Ns3TcpSocketImplDerived> socket = DynamicCast<Ns3TcpSocketImplDerived> (s);

  NS_TEST_ASSERT_MSG_EQ (socket->GetSocketType(), Socket::NS3_SOCK_STREAM,
                         "Different type of socket than expected");
  NS_TEST_ASSERT_MSG_EQ (socket->GetNode()->GetId(), m_node->GetId (),
                         "Different NodeID returned. Swapping of nodes?");
  NS_TEST_ASSERT_MSG_NE (socket, 0, "Can't cast to right type");

  // TODO in ns3TcpSocketImpl
  //int ret;
  //Address addr;
  //ret = m_socket->GetSockName(&addr);

  if (m_useIpv6 == true)
    {

    }
  else
    {
      CheckBind ();
    }
}

void
Ns3TcpSocketImplTest::ForwardUpCb()
{
  m_forwardUpCbInvoked = true;
}

void
Ns3TcpSocketImplTest::ForwardIcmpCb()
{
  m_forwardIcmpCbInvoked = true;
}

void
Ns3TcpSocketImplTest::DestroyCb()
{
  m_destroyCbInvoked = true;
}

void
Ns3TcpSocketImplTest::CheckBind()
{
  Ptr<TcpL4Protocol> tcp = m_node->GetObject<TcpL4Protocol> ();
  Ptr<Socket> s = tcp->CreateSocket (Ns3TcpSocketImplDerived::GetTypeId());

  Ptr<Ns3TcpSocketImplDerived> socket = DynamicCast<Ns3TcpSocketImplDerived> (s);

  int ret = socket->Bind();
  NS_TEST_ASSERT_MSG_GT_OR_EQ (ret, 0, "Bind failed with a valid address");

  if (ret >= 0)
    {
      NS_TEST_ASSERT_MSG_NE (socket->m_endPoint, 0,
                             "Endpoint is 0 but bind returned all ok");
      ObjectVectorValue v;
      tcp->GetAttribute("SocketList", v);

      NS_TEST_ASSERT_MSG_EQ (v.GetN(), 1,
                             "More TCP in the socket list than expected");
    }

  socket->SetForwardUpCallback  (MakeCallback(&Ns3TcpSocketImplTest::ForwardUpCb,
                                              this));
  socket->SetForwardIcmpCallback(MakeCallback (&Ns3TcpSocketImplTest::ForwardIcmpCb,
                                               this));
  socket->SetDestroyCallback    (MakeCallback (&Ns3TcpSocketImplTest::DestroyCb,
                                               this));

  Ptr<Packet> p = 0;
  Ipv4Header h;
  uint16_t sport = 0;
  Ptr<Ipv4Interface> a = 0;
  Ipv4Address b;

  socket->m_endPoint->ForwardUp(p, h, sport, a);

  //NS_TEST_ASSERT_MSG_EQ (m_forwardUpCbInvoked, true,
  //                       "ForwardUp callback on Tcp socket not invoked");

  //socket->m_endPoint->ForwardIcmp(b, 0, 0, 0, 0);

  //NS_TEST_ASSERT_MSG_EQ (m_forwardIcmpCbInvoked, true,
  //                       "ForwardIcmp callback on Tcp socket not invoked");

  //socket->m_endPoint = 0;
  //
  //NS_TEST_ASSERT_MSG_EQ (m_destroyCbInvoked, true,
  //                       "Destroy callback on Tcp socket not invoked");
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
