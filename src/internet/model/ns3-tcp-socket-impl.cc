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
#include <algorithm>

#include "ns3-tcp-socket-impl.h"
#include "ns3/abort.h"
#include "ns3/node.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/log.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/simulation-singleton.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include "ns3/trace-source-accessor.h"
#include "tcp-socket-base.h"
#include "tcp-l4-protocol.h"
#include "ipv4-end-point.h"
#include "ipv6-end-point.h"
#include "ipv6-l3-protocol.h"
#include "tcp-header.h"
#include "tcp-option-winscale.h"
#include "tcp-option-ts.h"
#include "rtt-estimator.h"

#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("Ns3TcpSocketImpl");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Ns3TcpSocketImpl);

Ns3TcpSocketImpl::Ns3TcpSocketImpl()
{

}

Ns3TcpSocketImpl::~Ns3TcpSocketImpl()
{

}

void
Ns3TcpSocketImpl::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void
Ns3TcpSocketImpl::SetTcp (Ptr<TcpL4Protocol> tcp)
{
  m_tcp = tcp;
}

void
Ns3TcpSocketImpl::SetRtt (Ptr<RttEstimator> rttEstimator)
{
  m_rttEstimator = rttEstimator;
}

enum Socket::SocketErrno
Ns3TcpSocketImpl::GetErrno (void) const
{
  return m_errno;
}

enum Socket::SocketType
Ns3TcpSocketImpl::GetSocketType (void) const
{
  return NS3_SOCK_STREAM;
}

Ptr<Node>
Ns3TcpSocketImpl::GetNode (void) const
{
  return m_node;
}

int
Ns3TcpSocketImpl::GetSockName (Address &address) const
{
  NS_LOG_FUNCTION (this << address);

  if (m_endPoint != 0)
    {
      address = InetSocketAddress (m_endPoint->GetLocalAddress (),
                                   m_endPoint->GetLocalPort ());
    }
  else if (m_endPoint6 != 0)
    {
      address = Inet6SocketAddress (m_endPoint6->GetLocalAddress (),
                                    m_endPoint6->GetLocalPort ());
    }
  else
    { // TODO
      // It is possible to call this method on a socket without a name
      // in which case, behavior is unspecified
      // Should this return an InetSocketAddress or an Inet6SocketAddress?
      address = InetSocketAddress (Ipv4Address::GetZero (), 0);
    }
  return 0;
}

void
Ns3TcpSocketImpl::BindToNetDevice (Ptr<NetDevice> netdevice)
{
  NS_LOG_FUNCTION (this << netdevice);

  Socket::BindToNetDevice (netdevice); // Includes sanity check

  if (m_endPoint == 0)
    {
      if (Bind () == -1)
        {
          NS_ASSERT (m_endPoint == 0);
          return;
        }
      NS_ASSERT (m_endPoint != 0);
    }

  m_endPoint->BindToNetDevice (netdevice);

  if (m_endPoint6 == 0)
    {
      if (Bind6 () == -1)
        {
          NS_ASSERT (m_endPoint6 == 0);
          return;
        }
      NS_ASSERT (m_endPoint6 != 0);
    }

  m_endPoint6->BindToNetDevice (netdevice);

  return;
}

/* Inherit from Socket class: Bind socket to an end-point in TcpL4Protocol */
int
Ns3TcpSocketImpl::Bind (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_tcp != 0);

  m_endPoint = m_tcp->Allocate ();

  if (0 == m_endPoint)
    {
      m_errno = ERROR_ADDRNOTAVAIL;
      return -1;
    }

  if (std::find(m_tcp->m_sockets.begin(), m_tcp->m_sockets.end(), this)
      == m_tcp->m_sockets.end())
    {
      m_tcp->m_sockets.push_back (this);
    }

  return SetupCallback ();
}

int
Ns3TcpSocketImpl::Bind6 (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_tcp != 0);

  m_endPoint6 = m_tcp->Allocate6 ();

  if (0 == m_endPoint6)
    {
      m_errno = ERROR_ADDRNOTAVAIL;
      return -1;
    }

  if (std::find(m_tcp->m_sockets.begin(), m_tcp->m_sockets.end(), this)
      == m_tcp->m_sockets.end())
    {
      m_tcp->m_sockets.push_back (this);
    }
  return SetupCallback ();
}

int
Ns3TcpSocketImpl::SetupCallback (void)
{
  NS_LOG_FUNCTION (this);

  if (m_endPoint == 0 && m_endPoint6 == 0)
    {
      return -1;
    }

  if (m_endPoint != 0)
    {
      m_endPoint->SetRxCallback      (MakeCallback (&Ns3TcpSocketImpl::ForwardUp,
                                                    Ptr<Ns3TcpSocketImpl> (this)));
      m_endPoint->SetIcmpCallback    (MakeCallback (&Ns3TcpSocketImpl::ForwardIcmp,
                                                    Ptr<Ns3TcpSocketImpl> (this)));
      m_endPoint->SetDestroyCallback (MakeCallback (&Ns3TcpSocketImpl::Destroy,
                                                    Ptr<Ns3TcpSocketImpl> (this)));
    }

  // TODO: What should happen if we have both endpoint?
  if (m_endPoint6 != 0)
    {
      m_endPoint6->SetRxCallback      (MakeCallback (&Ns3TcpSocketImpl::ForwardUp6,
                                                     Ptr<Ns3TcpSocketImpl> (this)));
      m_endPoint6->SetIcmpCallback    (MakeCallback (&Ns3TcpSocketImpl::ForwardIcmp6,
                                                     Ptr<Ns3TcpSocketImpl> (this)));
      m_endPoint6->SetDestroyCallback (MakeCallback (&Ns3TcpSocketImpl::Destroy6,
                                                     Ptr<Ns3TcpSocketImpl> (this)));
    }

  return 0;
}

int
Ns3TcpSocketImpl::Bind (const Address &address)
{
  NS_LOG_FUNCTION (this << address);

  if (InetSocketAddress::IsMatchingType (address))
    {
      InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
      Ipv4Address ipv4 = transport.GetIpv4 ();
      uint16_t port = transport.GetPort ();

      if (ipv4 == Ipv4Address::GetAny () && port == 0)
        {
          //m_endPoint = m_tcp->Allocate ();  --> old code
          return Bind ();
        }
      else if (ipv4 == Ipv4Address::GetAny () && port != 0)
        {
          m_endPoint = m_tcp->Allocate (port);
        }
      else if (ipv4 != Ipv4Address::GetAny () && port == 0)
        {
          m_endPoint = m_tcp->Allocate (ipv4);
        }
      else if (ipv4 != Ipv4Address::GetAny () && port != 0)
        {
          m_endPoint = m_tcp->Allocate (ipv4, port);
        }

      if (0 == m_endPoint)
        {
          m_errno = port ? ERROR_ADDRINUSE : ERROR_ADDRNOTAVAIL;
          return -1;
        }
    }
  else if (Inet6SocketAddress::IsMatchingType (address))
    {
      Inet6SocketAddress transport = Inet6SocketAddress::ConvertFrom (address);
      Ipv6Address ipv6 = transport.GetIpv6 ();
      uint16_t port = transport.GetPort ();

      if (ipv6 == Ipv6Address::GetAny () && port == 0)
        {
          // m_endPoint6 = m_tcp->Allocate6 (); --> old code
          return Bind6 ();
        }
      else if (ipv6 == Ipv6Address::GetAny () && port != 0)
        {
          m_endPoint6 = m_tcp->Allocate6 (port);
        }
      else if (ipv6 != Ipv6Address::GetAny () && port == 0)
        {
          m_endPoint6 = m_tcp->Allocate6 (ipv6);
        }
      else if (ipv6 != Ipv6Address::GetAny () && port != 0)
        {
          m_endPoint6 = m_tcp->Allocate6 (ipv6, port);
        }

      if (0 == m_endPoint6)
        {
          m_errno = port ? ERROR_ADDRINUSE : ERROR_ADDRNOTAVAIL;
          return -1;
        }
    }
  else
    {
      m_errno = ERROR_INVAL;
      return -1;
    }

  if (std::find(m_tcp->m_sockets.begin(), m_tcp->m_sockets.end(), this) ==
      m_tcp->m_sockets.end())
    {
      m_tcp->m_sockets.push_back (this);
    }

  NS_LOG_LOGIC ("Ns3TcpSocketImpl " << this << " got an endpoint: " << m_endPoint);

  return SetupCallback ();
}

int
Ns3TcpSocketImpl::Connect (const Address & address)
{
  NS_LOG_FUNCTION (this << address);

  // If haven't do so, Bind() this socket first
  if (InetSocketAddress::IsMatchingType (address) && m_endPoint6 == 0)
    {
      if (m_endPoint == 0)
        {
          if (Bind () == -1)
            {
              NS_ASSERT (m_endPoint == 0);
              return -1; // Bind() failed
            }
          NS_ASSERT (m_endPoint != 0);
        }
      InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
      m_endPoint->SetPeer (transport.GetIpv4 (), transport.GetPort ());
      m_endPoint6 = 0;

      /* Get the appropriate local address and port number from the routing
         protocol and set up endpoint */
      if (SetupEndpoint () != 0)
        { // Route to destination does not exist
          return -1;
        }
    }
  else if (Inet6SocketAddress::IsMatchingType (address)  && m_endPoint == 0)
    {
      // If we are operating on a v4-mapped address, translate the address to
      // a v4 address and re-call this function
      Inet6SocketAddress transport = Inet6SocketAddress::ConvertFrom (address);
      Ipv6Address v6Addr = transport.GetIpv6 ();
      if (v6Addr.IsIpv4MappedAddress () == true)
        {
          Ipv4Address v4Addr = v6Addr.GetIpv4MappedAddress ();
          return Connect (InetSocketAddress (v4Addr, transport.GetPort ()));
        }

      if (m_endPoint6 == 0)
        {
          if (Bind6 () == -1)
            {
              NS_ASSERT (m_endPoint6 == 0);
              return -1; // Bind() failed
            }
          NS_ASSERT (m_endPoint6 != 0);
        }
      m_endPoint6->SetPeer (v6Addr, transport.GetPort ());
      m_endPoint = 0;

      /* Get the appropriate local address and port number from the routing
      protocol and set up endpoint */
      if (SetupEndpoint6 () != 0)
        { // Route to destination does not exist
          return -1;
        }
    }
  else
    {
      m_errno = ERROR_INVAL;
      return -1;
    }

  // TODO RESET AFTER CLOSE
  // Re-initialize parameters in case this socket is being reused after CLOSE
  // m_rtt->Reset ();
  // m_cnCount = m_cnRetries;

  // DoConnect() will do state-checking and send a SYN packet
  return DoConnect ();
}

int
Ns3TcpSocketImpl::SetupEndpoint ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4> ();
  NS_ASSERT (ipv4 != 0);

  if (ipv4->GetRoutingProtocol () == 0)
    {
      NS_FATAL_ERROR ("No Ipv4RoutingProtocol in the node");
    }

  // Create a dummy packet, then ask the routing function for the best output
  // interface's address
  Ipv4Header header;
  header.SetDestination (m_endPoint->GetPeerAddress ());
  Socket::SocketErrno errno_;
  Ptr<Ipv4Route> route;
  Ptr<NetDevice> oif = m_boundnetdevice;
  route = ipv4->GetRoutingProtocol ()->RouteOutput (Ptr<Packet> (), header, oif,
                                                    errno_);
  if (route == 0)
    {
      NS_LOG_LOGIC ("Route to " << m_endPoint->GetPeerAddress () <<
                    " does not exist");
      NS_LOG_ERROR (errno_);
      m_errno = errno_;
      return -1;
    }

  NS_LOG_LOGIC ("Route exists");
  m_endPoint->SetLocalAddress (route->GetSource ());
  return 0;
}

int
Ns3TcpSocketImpl::SetupEndpoint6 ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol> ();
  NS_ASSERT (ipv6 != 0);

  if (ipv6->GetRoutingProtocol () == 0)
    {
      NS_FATAL_ERROR ("No Ipv6RoutingProtocol in the node");
    }
  // Create a dummy packet, then ask the routing function for the best output
  // interface's address
  Ipv6Header header;
  header.SetDestinationAddress (m_endPoint6->GetPeerAddress ());
  Socket::SocketErrno errno_;
  Ptr<Ipv6Route> route;
  Ptr<NetDevice> oif = m_boundnetdevice;
  route = ipv6->GetRoutingProtocol ()->RouteOutput (Ptr<Packet> (), header, oif,
                                                    errno_);
  if (route == 0)
    {
      NS_LOG_LOGIC ("Route to " << m_endPoint6->GetPeerAddress () <<
                    " does not exist");
      NS_LOG_ERROR (errno_);
      m_errno = errno_;
      return -1;
    }

  NS_LOG_LOGIC ("Route exists");
  m_endPoint6->SetLocalAddress (route->GetSource ());
  return 0;
}

int
Ns3TcpSocketImpl::Listen (void)
{
  NS_LOG_FUNCTION (this);
  // Linux quits EINVAL if we're not in CLOSED state, so match what they do
  if (m_state != CLOSED)
    {
      m_errno = ERROR_INVAL;
      return -1;
    }
  // In other cases, set the state to LISTEN and done
  NS_LOG_INFO ("CLOSED -> LISTEN");
  m_state = LISTEN;
  return 0;
}

int
Ns3TcpSocketImpl::Close (void)
{
  NS_LOG_FUNCTION (this);
  /// \internal
  /// First we check to see if there is any unread rx data.
  /// \bugid{426} claims we should send reset in this case.
  if (GetRxBufferSize () > 0)
    {
      NS_LOG_INFO ("Socket " << this << " << unread rx data during close. Sending reset");
      SendRST ();
      return 0;
    }

  if (HasPendingData ())
    {
      // App close with pending data must wait until all data transmitted
      if (m_closeOnEmpty == false)
        {
          m_closeOnEmpty = true;
          NS_LOG_INFO ("Socket " << this << " deferring close, state " <<
                       TcpStateName[m_state]);
        }

      return 0;
    }

  switch (m_state)
    {
    case SYN_RCVD:
    case ESTABLISHED:
      // send FIN to close the peer
      SendFIN ();
      NS_LOG_INFO ("ESTABLISHED -> FIN_WAIT_1");
      m_state = FIN_WAIT_1;
      break;
    case CLOSE_WAIT:
      // send FIN+ACK to close the peer
      SendFINACK ();
      NS_LOG_INFO ("CLOSE_WAIT -> LAST_ACK");
      m_state = LAST_ACK;
      break;
    case SYN_SENT:
    case CLOSING:
      // Send RST if application closes in SYN_SENT and CLOSING
      SendRST ();
      CloseAndNotify ();
      break;
    case LISTEN:
    case LAST_ACK:
      // In these three states, move to CLOSED and tear down the end point
      CloseAndNotify ();
      break;
    case CLOSED:
    case FIN_WAIT_1:
    case FIN_WAIT_2:
    case TIME_WAIT:
    default:
      // Do nothing in these four states
      break;
    }
  return 0;
}

void
Ns3TcpSocketImpl::CloseAndNotify (void)
{
  NS_LOG_FUNCTION (this);

  if (!m_closeNotified)
    {
      NotifyNormalClose ();
      m_closeNotified = true;
    }

  NS_LOG_INFO (TcpStateName[m_state] << " -> CLOSED");
  m_state = CLOSED;
  DeallocateEndPoint ();
}

void
Ns3TcpSocketImpl::DeallocateEndPoint (void)
{
  if (m_endPoint != 0)
    {
      CancelAllTimers ();
      m_endPoint->SetDestroyCallback (MakeNullCallback<void> ());
      m_tcp->DeAllocate (m_endPoint);
      m_endPoint = 0;
      std::vector<Ptr<Ns3TcpSocketImpl> >::iterator it
        = std::find (m_tcp->m_sockets.begin (), m_tcp->m_sockets.end (), this);
      if (it != m_tcp->m_sockets.end ())
        {
          m_tcp->m_sockets.erase (it);
        }
    }
  else if (m_endPoint6 != 0)
    {
      CancelAllTimers ();
      m_endPoint6->SetDestroyCallback (MakeNullCallback<void> ());
      m_tcp->DeAllocate (m_endPoint6);
      m_endPoint6 = 0;
      std::vector<Ptr<Ns3TcpSocketImpl> >::iterator it
        = std::find (m_tcp->m_sockets.begin (), m_tcp->m_sockets.end (), this);
      if (it != m_tcp->m_sockets.end ())
        {
          m_tcp->m_sockets.erase (it);
        }
    }
}

} // namespace ns-3
