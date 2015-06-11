
#ifndef DCCP_SOCKET_FACTORY_H
#define DCCP_SOCKET_FACTORY_H

#include "ns3/socket-factory.h"

namespace ns3 {

class Socket;
/**
* \ingroup socket
*
* \brief API to create DCCP socket instances
*
* This abstract class defines the API for DCCP socket factory.
* All DCCP implementations must provide an implementation of CreateSocket
* below.
*
* \see UdpSocketFactoryImpl
*/

class DccpSocketFactory : public SocketFactory
{
public:
  /**
  * \brief Get the type ID.
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);

};

} // namespace ns3

#endif /* DCCP_SOCKET_FACTORY_H */
