

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv6-routing-protocol.h"
#include "dccp-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv6-packet-info-tag.h"
#include "dccp-socket-impl.h"
#include "dccp-l4-protocol.h"
#include "ipv4-end-point.h"
#include "dccp-socket.h"
#include "dccp-header.h"
#include <limits>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DccpSocketImpl");

NS_OBJECT_ENSURE_REGISTERED (DccpSocketImpl);

// The correct maximum UDP message size is 65507, as determined by the following formula:
// 0xffff - (sizeof(IP Header) + sizeof(UDP Header)) = 65535-(20+8) = 65507
// \todo MAX_IPV4_UDP_DATAGRAM_SIZE is correct only for IPv4
static const uint32_t MAX_IPV4_DCCP_DATAGRAM_SIZE = 65507; //!< Maximum DCCP datagram size

// Add attributes generic to all UdpSockets to base class UdpSocket
TypeId
DccpSocketImpl::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DccpSocketImpl")
    .SetParent<DccpSocket> ()
    .SetGroupName ("Internet")
    .AddConstructor<DccpSocketImpl> ()
    .AddTraceSource ("Drop",
                     "Drop DCCP packet due to receive buffer overflow",
                     MakeTraceSourceAccessor (&DccpSocketImpl::m_dropTrace),
                     "ns3::Packet::TracedCallback")

  ;
  return tid;
}

DccpSocketImpl::DccpSocketImpl ()
  : m_endPoint (0),
    m_node (0),
    m_dccp (0),
    m_errno (ERROR_NOTERROR),
    m_shutdownSend (false),
    m_shutdownRecv (false),
    m_connected (false),
    m_rxAvailable (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

DccpSocketImpl::~DccpSocketImpl ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_node = 0;
  /**
  * Note: actually this function is called AFTER
  * DccpSocketImpl::Destroy
  * so the code below is unnecessary in normal operations
  */
  if (m_endPoint != 0)
    {
      NS_ASSERT (m_dccp != 0);
      /**
      * Note that this piece of code is a bit tricky:
      * when DeAllocate is called, it will call into
      * Ipv4EndPointDemux::Deallocate which triggers
      * a delete of the associated endPoint which triggers
      * in turn a call to the method DccpSocketImpl::Destroy below
      * will will zero the m_endPoint field.
      */
      NS_ASSERT (m_endPoint != 0);
      m_dccp->DeAllocate (m_endPoint);
      NS_ASSERT (m_endPoint == 0);
    }
  m_dccp = 0;
}

void
DccpSocketImpl::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = node;

}
void
DccpSocketImpl::SetDccp (Ptr<DccpL4Protocol> dccp)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_dccp = dccp;
}


enum Socket::SocketErrno
DccpSocketImpl::GetErrno (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_errno;
}

enum Socket::SocketType
DccpSocketImpl::GetSocketType (void) const
{
  return NS3_SOCK_SEQPACKET;
}

Ptr<Node>
DccpSocketImpl::GetNode (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_node;
}

void
DccpSocketImpl::Destroy (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_endPoint = 0;
}

/* Deallocate the end point and cancel all the timers */
void
DccpSocketImpl::DeallocateEndPoint (void)
{
  if (m_endPoint != 0)
    {
      m_endPoint->SetDestroyCallback (MakeNullCallback<void> ());
      m_dccp->DeAllocate (m_endPoint);
      m_endPoint = 0;
    }
}


int
DccpSocketImpl::FinishBind (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  bool done = false;
  if (m_endPoint != 0)
    {
      //m_endPoint->SetRxCallback (MakeCallback (&DccpSocketImpl::ForwardUp, Ptr<DccpSocketImpl> (this)));
      //m_endPoint->SetIcmpCallback (MakeCallback (&UdpSocketImpl::ForwardIcmp, Ptr<UdpSocketImpl> (this)));
      m_endPoint->SetDestroyCallback (MakeCallback (&DccpSocketImpl::Destroy, Ptr<DccpSocketImpl> (this)));
      done = true;
    }
  if (done)
    {
      return 0;
    }
  return -1;
}

int
DccpSocketImpl::Bind (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_endPoint = m_dccp->Allocate ();
  return FinishBind ();
}

int
DccpSocketImpl::Bind (const Address &address)
{
  NS_LOG_FUNCTION (this << address);

  if (InetSocketAddress::IsMatchingType (address))
    {

      InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
      Ipv4Address ipv4 = transport.GetIpv4 ();
      uint16_t port = transport.GetPort ();
      if (ipv4 == Ipv4Address::GetAny () && port == 0)
        {
          m_endPoint = m_dccp->Allocate ();
        }
      else if (ipv4 == Ipv4Address::GetAny () && port != 0)
        {
          m_endPoint = m_dccp->Allocate (port);
        }
      else if (ipv4 != Ipv4Address::GetAny () && port == 0)
        {
          m_endPoint = m_dccp->Allocate (ipv4);
        }
      else if (ipv4 != Ipv4Address::GetAny () && port != 0)
        {
          m_endPoint = m_dccp->Allocate (ipv4, port);
        }
      if (0 == m_endPoint)
        {
          m_errno = port ? ERROR_ADDRINUSE : ERROR_ADDRNOTAVAIL;
          return -1;
        }
    }

  else
    {
      NS_LOG_ERROR ("Not IsMatchingType");
      m_errno = ERROR_INVAL;
      return -1;
    }

  return FinishBind ();
}

int
DccpSocketImpl::ShutdownSend (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_shutdownSend = true;
  return 0;
}

int
DccpSocketImpl::ShutdownRecv (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_shutdownRecv = true;
  return 0;
}

int
DccpSocketImpl::Close (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_shutdownRecv == true && m_shutdownSend == true)
    {
      m_errno = Socket::ERROR_BADF;
      return -1;
    }
  m_shutdownRecv = true;
  m_shutdownSend = true;
  DeallocateEndPoint ();
  return 0;
}

int
DccpSocketImpl::Connect (const Address & address)
{
  NS_LOG_FUNCTION (this << address);
  if (InetSocketAddress::IsMatchingType(address) == true)
    {
      InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
      m_defaultAddress = Address(transport.GetIpv4 ());
      m_defaultPort = transport.GetPort ();
      m_connected = true;
      NotifyConnectionSucceeded ();
    }
  else
    {
      return -1;
    }

  return 0;
}

int
DccpSocketImpl::Listen (void)
{
  m_errno = Socket::ERROR_OPNOTSUPP;
  return -1;
}

int
DccpSocketImpl::Send (Ptr<Packet> p, uint32_t flags)
{
  NS_LOG_FUNCTION (this << p << flags);

  if (!m_connected)
    {
      m_errno = ERROR_NOTCONN;
      return -1;
    }

  return DoSend (p);
}

int
DccpSocketImpl::DoSend (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  if ((m_endPoint == 0) && (InetSocketAddress::IsMatchingType(m_defaultAddress) == true))
    {
      if (Bind () == -1)
        {
          NS_ASSERT (m_endPoint == 0);
          return -1;
        }
      NS_ASSERT (m_endPoint != 0);
    }
  if (m_shutdownSend)
    {
      m_errno = ERROR_SHUTDOWN;
      return -1;
    }

  return DoSendTo (p, (const Address)m_defaultAddress);
}

int
DccpSocketImpl::DoSendTo (Ptr<Packet> p, const Address &address)
{
  NS_LOG_FUNCTION (this << p << address);

  if (!m_connected)
    {
      NS_LOG_LOGIC ("Not connected");
      if (InetSocketAddress::IsMatchingType(address) == true)
        {
          InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
          Ipv4Address ipv4 = transport.GetIpv4 ();
          uint16_t port = transport.GetPort ();
          return DoSendTo (p, ipv4, port);
        }
      else
        {
          return -1;
        }
    }
  else
    {
      // connected DCCP socket must use default addresses
      NS_LOG_LOGIC ("Connected");
      if (Ipv4Address::IsMatchingType(m_defaultAddress))
        {
          return DoSendTo (p, Ipv4Address::ConvertFrom(m_defaultAddress), m_defaultPort);
        }
    }
  m_errno = ERROR_AFNOSUPPORT;
  return(-1);
}

int
DccpSocketImpl::DoSendTo (Ptr<Packet> p, Ipv4Address dest, uint16_t port)
{
  NS_LOG_FUNCTION (this << p << dest << port);

  if (m_endPoint == 0)
    {
      if (Bind () == -1)
        {
          NS_ASSERT (m_endPoint == 0);
          return -1;
        }
      NS_ASSERT (m_endPoint != 0);
    }
  if (m_shutdownSend)
    {
      m_errno = ERROR_SHUTDOWN;
      return -1;
    }

  if (p->GetSize () > GetTxAvailable () )
    {
      m_errno = ERROR_MSGSIZE;
      return -1;
    }

  if (IsManualIpTos ())
    {
      SocketIpTosTag ipTosTag;
      ipTosTag.SetTos (GetIpTos ());
      p->AddPacketTag (ipTosTag);
    }

  Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4> ();
    
  // Locally override the IP TTL for this socket
  // We cannot directly modify the TTL at this stage, so we set a Packet tag
  // The destination can be either multicast, unicast/anycast, or
  // either all-hosts broadcast or limited (subnet-directed) broadcast.
  // For the latter two broadcast types, the TTL will later be set to one
  // irrespective of what is set in these socket options.  So, this tagging
  // may end up setting the TTL of a limited broadcast packet to be
  // the same as a unicast, but it will be fixed further down the stack
  DccpHeader dccpHeader;

  if (m_endPoint->GetLocalAddress () != Ipv4Address::GetAny ())
    {
      //*------------------------------------------------------------------------------------------------------------------------------SEND
      dccpHeader.SetSourcePort(m_endPoint->GetLocalPort ());
      dccpHeader.SetDestinationPort(port);
      dccpHeader.SetDataOffset(uint8_t(4));
      dccpHeader.SetCCVal(0);
      dccpHeader.SetCsCov(0);
      dccpHeader.SetRes(0);
      dccpHeader.SetType(0);
      dccpHeader.SetX(1);
      dccpHeader.SetSequenceNumberLow(SequenceNumber32(33));
      dccpHeader.SetSequenceNumberHigh(SequenceNumber16(0));
      dccpHeader.SetServiceCode (11111111);


      m_dccp->Send (p->Copy (), m_endPoint->GetLocalAddress (), dest, m_endPoint->GetLocalPort (), port, 0,dccpHeader);
      NotifyDataSent (p->GetSize ());
      NotifySend (GetTxAvailable ());
      return p->GetSize ();
    }
  else if (ipv4->GetRoutingProtocol () != 0)
    {
      Ipv4Header header;
      header.SetDestination (dest);
      header.SetProtocol (DccpL4Protocol::PROT_NUMBER);
      Socket::SocketErrno errno_;
      Ptr<Ipv4Route> route;
      Ptr<NetDevice> oif = m_boundnetdevice; //specify non-zero if bound to a specific device
      // TBD-- we could cache the route and just check its validity
      route = ipv4->GetRoutingProtocol ()->RouteOutput (p, header, oif, errno_);
      if (route != 0)
        {
          NS_LOG_LOGIC ("Route exists");

           uint32_t outputIfIndex = ipv4->GetInterfaceForDevice (route->GetOutputDevice ());
           uint32_t ifNAddr = ipv4->GetNAddresses (outputIfIndex);
           for (uint32_t addrI = 0; addrI < ifNAddr; ++addrI)
              {
                  Ipv4InterfaceAddress ifAddr = ipv4->GetAddress (outputIfIndex, addrI);
                  if (dest == ifAddr.GetBroadcast ())
                    {
                      m_errno = ERROR_OPNOTSUPP;
                      return -1;
                    }
                }

          header.SetSource (route->GetSource ());
          //*------------------------------------------------------------------------------------------------------------------------------SEND
          dccpHeader.SetSourcePort(m_endPoint->GetLocalPort ());
          dccpHeader.SetDestinationPort(port);
          dccpHeader.SetDataOffset(4);
          dccpHeader.SetCCVal(0);
          dccpHeader.SetCsCov(0);
          dccpHeader.SetRes(0);
          dccpHeader.SetType(0);
          dccpHeader.SetX(1);
          dccpHeader.SetSequenceNumberLow(SequenceNumber32(33));
          dccpHeader.SetSequenceNumberHigh(SequenceNumber16(0));
          dccpHeader.SetServiceCode (11111111);
          m_dccp->Send (p->Copy (), header.GetSource (), header.GetDestination (), m_endPoint->GetLocalPort (), port, route,dccpHeader);
          NotifyDataSent (p->GetSize ());
          return p->GetSize ();
        }
      else
        {
          NS_LOG_LOGIC ("No route to destination");
          NS_LOG_ERROR (errno_);
          m_errno = errno_;
          return -1;
        }
    }
  else
    {
      NS_LOG_ERROR ("ERROR_NOROUTETOHOST");
      m_errno = ERROR_NOROUTETOHOST;
      return -1;
    }

  return 0;
}

    
// maximum message size for UDP broadcast is limited by MTU
// size of underlying link; we are not checking that now.
// \todo Check MTU size of underlying link
uint32_t
DccpSocketImpl::GetTxAvailable (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  // No finite send buffer is modelled, but we must respect
  // the maximum size of an IP datagram (65535 bytes - headers).
  return MAX_IPV4_DCCP_DATAGRAM_SIZE;
}

int
DccpSocketImpl::SendTo (Ptr<Packet> p, uint32_t flags, const Address &address)
{
  NS_LOG_FUNCTION (this << p << flags << address);
  if (InetSocketAddress::IsMatchingType (address))
    {
      if (IsManualIpTos ())
        {
          SocketIpTosTag ipTosTag;
          ipTosTag.SetTos (GetIpTos ());
          p->AddPacketTag (ipTosTag);
        }

      InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
      Ipv4Address ipv4 = transport.GetIpv4 ();
      uint16_t port = transport.GetPort ();
      return DoSendTo (p, ipv4, port);
    }
  return -1;
}

uint32_t
DccpSocketImpl::GetRxAvailable (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  // We separately maintain this state to avoid walking the queue
  // every time this might be called
  return m_rxAvailable;
}

Ptr<Packet>
DccpSocketImpl::Recv (uint32_t maxSize, uint32_t flags)
{
  NS_LOG_FUNCTION (this << maxSize << flags);
  if (m_deliveryQueue.empty () )
    {
      m_errno = ERROR_AGAIN;
      return 0;
    }
  Ptr<Packet> p = m_deliveryQueue.front ();
  if (p->GetSize () <= maxSize)
    {
      m_deliveryQueue.pop ();
      m_rxAvailable -= p->GetSize ();
    }
  else
    {
      p = 0;
    }
  return p;
}

Ptr<Packet>
DccpSocketImpl::RecvFrom (uint32_t maxSize, uint32_t flags,
                         Address &fromAddress)
{
  NS_LOG_FUNCTION (this << maxSize << flags);
  Ptr<Packet> packet = Recv (maxSize, flags);
  if (packet != 0)
    {
      SocketAddressTag tag;
      bool found;
      found = packet->PeekPacketTag (tag);
      NS_ASSERT (found);
      fromAddress = tag.GetAddress ();
    }
  return packet;
}

int
DccpSocketImpl::GetSockName (Address &address) const
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_endPoint != 0)
    {
      address = InetSocketAddress (m_endPoint->GetLocalAddress (), m_endPoint->GetLocalPort ());
    }
  else
    { // It is possible to call this method on a socket without a name
      // in which case, behavior is unspecified
      // Should this return an InetSocketAddress or an Inet6SocketAddress?
      address = InetSocketAddress (Ipv4Address::GetZero (), 0);
    }
  return 0;
}


void
DccpSocketImpl::SetRcvBufSize (uint32_t size)
{
  m_rcvBufSize = size;
}

uint32_t
DccpSocketImpl::GetRcvBufSize (void) const
{
  return m_rcvBufSize;
}


bool
DccpSocketImpl::SetAllowBroadcast (bool allowBroadcast)
{
  return 0;
}

bool
DccpSocketImpl::GetAllowBroadcast () const
{
  return 0;
}


int
DccpSocketImpl::Bind6 (void)
{
  return 0;
}

void
DccpSocketImpl::BindToNetDevice (Ptr<NetDevice> netdevice)
{
}

void
DccpSocketImpl::SetMtuDiscover (bool discover)
{

}

bool
DccpSocketImpl::GetMtuDiscover (void) const
{
  return false;
}

}
// namespace ns3

