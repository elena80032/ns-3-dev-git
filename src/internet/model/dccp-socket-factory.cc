
#include "dccp-socket-factory.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DccpSocketFactory);

TypeId DccpSocketFactory::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DccpSocketFactory")
    .SetParent<SocketFactory> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

} // namespace ns3


