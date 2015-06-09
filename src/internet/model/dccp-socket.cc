#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "dccp-socket.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DccpSocket");

NS_OBJECT_ENSURE_REGISTERED (DccpSocket);

TypeId
DccpSocket::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DccpSocket")
    .SetParent<Socket> ()
    .SetGroupName ("Internet")
    .AddAttribute ("RcvBufSize",
                   "DccpSocket maximum receive buffer size (bytes)",
                   UintegerValue (131072),
                   MakeUintegerAccessor (&DccpSocket::GetRcvBufSize,
                                         &DccpSocket::SetRcvBufSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("IpTtl",
                   "socket-specific TTL for unicast IP packets (if non-zero)",
                   UintegerValue (0),
                   MakeUintegerAccessor (&DccpSocket::GetIpTtl,
                                         &DccpSocket::SetIpTtl),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("MtuDiscover", "If enabled, every outgoing ip packet will have the DF flag set.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&DccpSocket::SetMtuDiscover,
                                        &DccpSocket::GetMtuDiscover),
                   MakeBooleanChecker ())
  ;
  return tid;
}

DccpSocket::DccpSocket ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

DccpSocket::~DccpSocket ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

} // namespace ns3
