
#ifndef DCCP_SOCKET_FACTORY_IMPL_H
#define DCCP_SOCKET_FACTORY_IMPL_H

#include "dccp-socket-factory.h"
#include "ns3/ptr.h"

namespace ns3 {

class DccpL4Protocol;

class DccpSocketFactoryImpl : public DccpSocketFactory
{
public:
  //TypeId GetTypeId (void);
  DccpSocketFactoryImpl ();
  virtual ~DccpSocketFactoryImpl ();

  void SetDccp (Ptr<DccpL4Protocol> dccp);

  virtual Ptr<Socket> CreateSocket (void);

protected:
  virtual void DoDispose (void);
private:
  Ptr<DccpL4Protocol> m_dccp;
};

} // namespace ns3

#endif /* UDP_SOCKET_FACTORY_IMPL_H */
