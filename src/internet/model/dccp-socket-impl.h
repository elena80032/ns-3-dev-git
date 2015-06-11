#ifndef DCCPSOCKETIMPL_H
#define DCCPSOCKETIMPL_H

#include <stdint.h>
#include <queue>
#include "ns3/callback.h"
#include "ns3/traced-callback.h"
#include "ns3/socket.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "dccp-socket.h"
#include "ns3/ipv4-interface.h"
#include "icmpv4.h"

namespace ns3 {

class Ipv4EndPoint;
class Ipv6EndPoint;
class Node;
class Packet;
class DccpL4Protocol;
class DccpHeader;

class DccpSocketImpl : public DccpSocket
{
public:

  static TypeId GetTypeId (void);
  DccpSocketImpl ();
  virtual ~DccpSocketImpl ();

  void SetNode (Ptr<Node> node);

  void SetDccp (Ptr<DccpL4Protocol> dccp);

  virtual enum SocketErrno GetErrno (void) const;
  virtual enum SocketType GetSocketType (void) const;
  virtual Ptr<Node> GetNode (void) const;
  virtual int Bind (void);
  virtual int Bind6 (void);
  virtual int Bind (const Address &address);
  virtual int Close (void);
  virtual int ShutdownSend (void);
  virtual int ShutdownRecv (void);
  virtual int Connect (const Address &address);
  virtual int Listen (void);
  virtual int Send (Ptr<Packet> p, uint32_t flags);
  virtual int SendTo (Ptr<Packet> p, uint32_t flags, const Address &address);
  virtual uint32_t GetTxAvailable (void) const;
  virtual Ptr<Packet> Recv (uint32_t maxSize, uint32_t flags);
  virtual Ptr<Packet> RecvFrom (uint32_t maxSize, uint32_t flags,
                                Address &fromAddress);
  virtual int GetSockName (Address &address) const;
  virtual void BindToNetDevice (Ptr<NetDevice> netdevice);
  virtual uint32_t GetRxAvailable (void) const;
  virtual bool SetAllowBroadcast (bool allowBroadcast);
  virtual bool GetAllowBroadcast () const;
private:
  // Attributes set through UdpSocket base class
  virtual void SetRcvBufSize (uint32_t size);
  virtual uint32_t GetRcvBufSize (void) const;
  virtual void SetMtuDiscover (bool discover);
  virtual bool GetMtuDiscover (void) const;

  friend class DccpSocketFactory;

  int FinishBind (void);
  void Destroy (void);
  void DeallocateEndPoint (void);
  int DoSend (Ptr<Packet> p);
  int DoSendTo (Ptr<Packet> p, const Address &daddr);
  int DoSendTo (Ptr<Packet> p, Ipv4Address daddr, uint16_t dport);

  // Connections to other layers of TCP/IP
  Ipv4EndPoint*       m_endPoint;   //!< the IPv4 endpoint
  Ptr<Node>           m_node;       //!< the associated node
  Ptr<DccpL4Protocol> m_dccp;         //!< the associated UDP L4 protocol


  Address m_defaultAddress; //!< Default address
  uint16_t m_defaultPort;   //!< Default port
  TracedCallback<Ptr<const Packet> > m_dropTrace; //!< Trace for dropped packets

  enum SocketErrno         m_errno;           //!< Socket error code
  bool                     m_shutdownSend;    //!< Send no longer allowed
  bool                     m_shutdownRecv;    //!< Receive no longer allowed
  bool                     m_connected;       //!< Connection established

  std::queue<Ptr<Packet> > m_deliveryQueue; //!< Queue for incoming packets
  uint32_t m_rxAvailable;                   //!< Number of available bytes to be received

  // Socket attributes
  uint32_t m_rcvBufSize;    //!< Receive buffer size
  bool m_mtuDiscover;       //!< Allow MTU discovery
};

} // namespace ns3

#endif /* DCCP_SOCKET_IMPL_H */
