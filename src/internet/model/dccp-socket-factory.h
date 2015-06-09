
#ifndef DCCP_SOCKET_FACTORY_H
#define DCCP_SOCKET_FACTORY_H

#include "ns3/socket-factory.h"

namespace ns3 {

class Socket;

class DccpSocketFactory : public SocketFactory
{
public:

  static TypeId GetTypeId (void);

};

} // namespace ns3

#endif /* DCCP_SOCKET_FACTORY_H */
