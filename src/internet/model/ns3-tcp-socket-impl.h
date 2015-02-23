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
#ifndef NS3TCPSOCKETIMPL_H
#define NS3TCPSOCKETIMPL_H

#include "tcp-socket.h"
#include <stdint.h>
#include <queue>
#include "ns3/callback.h"
#include "ns3/traced-value.h"
#include "ns3/tcp-socket.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-interface.h"
#include "ns3/event-id.h"
#include "tcp-tx-buffer.h"
#include "tcp-rx-buffer.h"
#include "rtt-estimator.h"

namespace ns3 {

class Ipv4EndPoint;
class Ipv6EndPoint;
class Node;
class Packet;
class TcpL4Protocol;
class TcpHeader;

class Ns3TcpSocketImpl : public TcpSocket
{
public:
  Ns3TcpSocketImpl ();
  ~Ns3TcpSocketImpl ();

  /**
   * \brief Set the associated node.
   * \param node the node
   */
  virtual void SetNode (Ptr<Node> node);

  /**
   * \brief Set the associated TCP L4 protocol.
   * \param tcp the TCP L4 protocol
   */
  virtual void SetTcp (Ptr<TcpL4Protocol> tcp);

  /**
   * \brief Set the associated RTT estimator.
   * \param rtt the RTT estimator
   */
  virtual void SetRtt (Ptr<RttEstimator> rtt);

  // Necessary implementations of null functions from ns3::Socket
  virtual enum SocketErrno GetErrno        (void) const;
  virtual enum SocketType  GetSocketType   (void) const;
  virtual Ptr<Node>        GetNode         (void) const;
  virtual int              GetSockName     (Address &address) const;
  virtual void             BindToNetDevice (Ptr<NetDevice> netdevice);
  virtual int              Bind            (void);
  virtual int              Bind6           (void);
  virtual int              Bind            (const Address &address);
  virtual int              Connect         (const Address &address);
  virtual int              Listen          (void);
  virtual int              Close           (void);

  // A gently reminder of what function should be implemented
  virtual int              ShutdownSend    (void) = 0;
  virtual int              ShutdownRecv    (void) = 0;

  virtual int              Send            (Ptr<Packet> p, uint32_t flags) = 0;
  virtual int              SendTo          (Ptr<Packet> p, uint32_t flags,
                                            const Address &toAddress) = 0;
  virtual Ptr<Packet>      Recv            (uint32_t maxSize, uint32_t flags) = 0;
  virtual Ptr<Packet>      RecvFrom        (uint32_t maxSize, uint32_t flags,
                                            Address &fromAddress) = 0;
  virtual uint32_t         GetTxAvailable  (void) const = 0;
  virtual uint32_t         GetRxAvailable  (void) const = 0;

protected:

  // State-related attributes
  TracedValue<TcpStates_t> m_state;         //!< TCP state
  enum SocketErrno         m_errno;         //!< Socket error code

  // Connections to other layers of TCP/IP
  Ipv4EndPoint*       m_endPoint;   //!< the IPv4 endpoint
  Ipv6EndPoint*       m_endPoint6;  //!< the IPv6 endpoint
  Ptr<Node>           m_node;       //!< the associated node
  Ptr<TcpL4Protocol>  m_tcp;        //!< the associated TCP L4 protocol
  Ptr<RttEstimator>   m_rttEstimator; //!< Round trip time estimator


  bool                     m_closeNotified; //!< Told app to close socket
  bool                     m_closeOnEmpty;  //!< Close socket upon tx buffer emptied
protected:
  /**
   * \brief Called by the L3 protocol when it received a packet to pass on to TCP.
   *
   * \param packet the incoming packet
   * \param header the packet's IPv4 header
   * \param port the remote port
   * \param incomingInterface the incoming interface
   */
  virtual void ForwardUp (Ptr<Packet> packet, Ipv4Header header, uint16_t port,
                          Ptr<Ipv4Interface> incomingInterface) = 0;

  /**
   * \brief Called by the L3 protocol when it received a packet to pass on to TCP.
   *
   * \param packet the incoming packet
   * \param header the packet's IPv6 header
   * \param port the remote port
   * \param incomingInterface the incoming interface
   */
  virtual void ForwardUp6 (Ptr<Packet> packet, Ipv6Header header, uint16_t port,
                           Ptr<Ipv6Interface> incomingInterface) = 0;

  /**
   * \brief Called by the L3 protocol when it received an ICMP packet to pass on
   * to TCP.
   *
   * \param icmpSource the ICMP source address
   * \param icmpTtl the ICMP Time to Live
   * \param icmpType the ICMP Type
   * \param icmpCode the ICMP Code
   * \param icmpInfo the ICMP Info
   */
  virtual void ForwardIcmp (Ipv4Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                            uint8_t icmpCode, uint32_t icmpInfo) = 0;

  /**
   * \brief Called by the L3 protocol when it received an ICMPv6 packet to pass
   * on to TCP.
   *
   * \param icmpSource the ICMP source address
   * \param icmpTtl the ICMP Time to Live
   * \param icmpType the ICMP Type
   * \param icmpCode the ICMP Code
   * \param icmpInfo the ICMP Info
   */
  virtual void ForwardIcmp6 (Ipv6Address icmpSource, uint8_t icmpTtl, uint8_t icmpType,
                             uint8_t icmpCode, uint32_t icmpInfo) = 0;

  /**
   * \brief Kill this socket by zeroing its attributes (IPv4)
   *
   * This is a callback function configured to m_endpoint in
   * SetupCallback(), invoked when the endpoint is destroyed.
   */
  virtual void Destroy (void) = 0;

  /**
   * \brief Kill this socket by zeroing its attributes (IPv6)
   *
   * This is a callback function configured to m_endpoint in
   * SetupCallback(), invoked when the endpoint is destroyed.
   */
  virtual void Destroy6 (void) = 0;

  // Helper functions: Connection set up

  /**
   * \brief Common part of the two Bind(), i.e. set callback and remembering
   * local addr:port
   *
   * \returns 0 on success, -1 on failure
   */
  int SetupCallback (void);

  /**
   * \brief Configure the endpoint to a local address. Called by Connect() if Bind() didn't specify one.
   *
   * \returns 0 on success
   */
  int SetupEndpoint (void);

  /**
   * \brief Configure the endpoint v6 to a local address. Called by Connect() if Bind() didn't specify one.
   *
   * \returns 0 on success
   */
  int SetupEndpoint6 (void);

  /**
   * \brief Perform the real connection tasks: Send SYN if allowed, RST if invalid
   *
   * \returns 0 on success
   */
  virtual int DoConnect (void) = 0;

  /**
   * \brief Return the RxBuffer (actual) size in byte
   * \return Bytes in the RxBuffer
   */
  virtual uint32_t GetRxBufferSize (void) const = 0;

  /**
   * \brief Check if we have some other data to send
   * \return true if we have data to send in the buffer
   */
  virtual bool HasPendingData (void) const = 0;

  /**
   * \brief Peacefully close the socket by notifying the upper layer and
   * deallocate end point
   */
  void CloseAndNotify (void);

  /**
   * \brief Deallocate m_endPoint and m_endPoint6
   */
  void DeallocateEndPoint (void);

  /**
   * \brief Cancel all timer when endpoint is deleted
   */
  virtual void CancelAllTimers (void) = 0;

  /**
   * \brief Send an RST packet to the peer
   */
  virtual void SendRST    (void) = 0;
  virtual void SendFIN    (void) = 0;
  virtual void SendFINACK (void) = 0;
};

} // namespace ns3

#endif // NS3TCPSOCKETIMPL_H
