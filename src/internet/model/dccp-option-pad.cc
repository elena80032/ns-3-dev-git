#include "ns3/log.h"
#include "ns3/type-id.h"
#include <vector>
#include "dccp-option-pad.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DccpOptionPadding");

NS_OBJECT_ENSURE_REGISTERED (DccpOptionPadding);

DccpOptionPadding::DccpOptionPadding ()
  : DccpOption ()
{
}

DccpOptionPadding::~DccpOptionPadding ()
{
}

TypeId
DccpOptionPadding::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DccpOptionPadding")
    .SetParent<DccpOption> ()
    .SetGroupName ("Internet")
    .AddConstructor<DccpOptionPadding> ()
  ;
  return tid;
}

TypeId
DccpOptionPadding::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


uint8_t
DccpOptionPadding::GetKind (void) const
{
  return DccpOption::PADDING;
}

void
DccpOptionPadding::Print (std::ostream &os) const
{
  os << "Padding option";
}

uint8_t
DccpOptionPadding::GetSerializedSize (void) const
{
  return 1;
}

void
DccpOptionPadding::Serialize (Buffer::Iterator i) const
{
  // Write 00000000
  i.WriteU8 (GetKind ());
}

uint32_t
DccpOptionPadding::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  i.ReadU8 ();
  return 1;
}

} // namespace ns3

