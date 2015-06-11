#include "dccp-socket-factory-impl.h"
#include "dccp-l4-protocol.h"
#include "ns3/socket.h"
#include "ns3/assert.h"

namespace ns3 {

DccpSocketFactoryImpl::DccpSocketFactoryImpl ()
  : m_dccp (0)
{
}
DccpSocketFactoryImpl::~DccpSocketFactoryImpl ()
{
  NS_ASSERT (m_dccp == 0);
}

void
DccpSocketFactoryImpl::SetDccp (Ptr<DccpL4Protocol> dccp)
{
  m_dccp = dccp;
}

Ptr<Socket>
DccpSocketFactoryImpl::CreateSocket (void)
{
  return m_dccp->CreateSocket ();
}

void
DccpSocketFactoryImpl::DoDispose (void)
{
  m_dccp = 0;
  DccpSocketFactory::DoDispose ();
}

} // namespace ns3
