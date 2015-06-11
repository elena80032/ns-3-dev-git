
#ifndef DCCP_SOCKET_FACTORY_IMPL_H
#define DCCP_SOCKET_FACTORY_IMPL_H

#include "dccp-socket-factory.h"
#include "ns3/ptr.h"

namespace ns3 {

class DccpL4Protocol;
/**
* \ingroup dccp
* \brief Object to create DCCP socket instances
*
* This class implements the API for creating DCCP sockets.
* It is a socket factory (deriving from class SocketFactory).
*/

class DccpSocketFactoryImpl : public DccpSocketFactory
{
public:
  //TypeId GetTypeId (void);
  DccpSocketFactoryImpl ();
  virtual ~DccpSocketFactoryImpl ();

  /**
  * \brief Set the associated DCCP L4 protocol.
  * \param udp the DCCP L4 protocol
  */
  void SetDccp (Ptr<DccpL4Protocol> dccp);

  /**
  * \brief Implements a method to create a Dccp-based socket and return
  * a base class smart pointer to the socket.
  *
  * \return smart pointer to Socket
  */
  virtual Ptr<Socket> CreateSocket (void);

protected:
  virtual void DoDispose (void);
private:
  Ptr<DccpL4Protocol> m_dccp;//!< the associated DCCP L4 protocol
};

} // namespace ns3

#endif /* UDP_SOCKET_FACTORY_IMPL_H */
