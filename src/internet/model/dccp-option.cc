#include "dccp-option.h"
#include "ns3/log.h"
#include "ns3/type-id.h"
#include "dccp-option-pad.h"
#include <vector>

namespace ns3 {


NS_LOG_COMPONENT_DEFINE ("DccpOption");

NS_OBJECT_ENSURE_REGISTERED (DccpOption);

DccpOption::DccpOption ()
{
}

DccpOption::~DccpOption ()
{
}

TypeId
DccpOption::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DccpOption")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId
DccpOption::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

Ptr<DccpOption>
DccpOption::CreateOption (uint8_t kind)
{
    struct kindToTid
    {
      DccpOption::Kind kind;
      TypeId tid;
    };

    static ObjectFactory objectFactory;
    static kindToTid toTid[] =
    {
      { DccpOption::PADDING,  DccpOptionPadding::GetTypeId () },
      { DccpOption::UNKNOWN,  DccpOptionUnknown::GetTypeId () }
    };

    for (unsigned int i = 0; i < sizeof (toTid) / sizeof (kindToTid); ++i)
      {
        if (toTid[i].kind == kind)
          {
            objectFactory.SetTypeId (toTid[i].tid);
            return objectFactory.Create<DccpOption> ();
          }
      }

    return CreateObject<DccpOptionUnknown> ();
  }


bool
DccpOption::IsKindKnown (uint8_t kind)
{
  switch (kind)
    {
    case PADDING:
      return true;
    }

  return false;
}


NS_OBJECT_ENSURE_REGISTERED (DccpOptionUnknown);

DccpOptionUnknown::DccpOptionUnknown ()
  : DccpOption ()
{
  m_kind = 0xFF;
  m_size = 0;
}

DccpOptionUnknown::~DccpOptionUnknown ()
{
}

TypeId
DccpOptionUnknown::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DccpOptionUnknown")
    .SetParent<DccpOption> ()
    .SetGroupName ("Internet")
    .AddConstructor<DccpOptionUnknown> ()
  ;
  return tid;
}

TypeId
DccpOptionUnknown::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DccpOptionUnknown::Print (std::ostream &os) const
{
  os << "Unknown option";
}

uint8_t
DccpOptionUnknown::GetSerializedSize (void) const
{
  return m_size;
}

void
DccpOptionUnknown::Serialize (Buffer::Iterator i) const
{
  if (m_size == 0)
    {
      NS_LOG_WARN ("Can't Serialize an Unknown Dccp Option");
      return;
    }
  i.WriteU8 (GetKind ());
  if (GetSerializedSize () != 1)
  {
    i.WriteU8 (GetSerializedSize ());
    i.Write (m_content, m_size-2);
  }
}

uint32_t
DccpOptionUnknown::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_kind = i.ReadU8 ();
  if (m_kind >= 0 && m_kind <= 31)
  {
     m_size = 1;
  }
  else
  {
      m_size = i.ReadU8 ();
      if (m_size < 2)
        {
          NS_LOG_WARN ("Unable to parse an unknown option of kind " << int (m_kind) << " with apparent size " << int (m_size));
          return 0;
        }
      i.Read (m_content, m_size-2);
  }
  return m_size;
}

uint8_t
DccpOptionUnknown::GetKind (void) const
{
  return m_kind;
}

}
