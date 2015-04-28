#include "dccp-header.h"

namespace ns3 {
  NS_OBJECT_ENSURE_REGISTERED (DccpHeader);

DccpHeader::DccpHeader()
  :
  m_dataOffset(0),
  m_CCVal_CsCov(0),
  m_RTXRSn(0),
  m_sourcePort(0),
  m_destinationPort(0),
  m_sequenceNumberLow(0)

{
}

DccpHeader::~DccpHeader ()
{
}
/*
//-----------------------------------------------------------------------------Serialize
uint32_t
DccpHeader::GetSerializedSize (void) const
{
  if (GetX())
    {
      return 16;
    }
  else
    {
      return 12;
    }
}

void
DccpHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_sourcePort);
  i.WriteHtonU16 (m_destinationPort);
  i.WriteU8(m_dataOffset);
  i.WriteU8(m_CCVal_CsCov);

  /*
  if ( m_checksum == 0)
    {
      i.WriteU16 (0);

      if (m_calcChecksum)
        {
          uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
          i = start;
          uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);
          i = start;
          i.Next (6);
          i.WriteU16 (checksum);
        }
    }
  else
    {
      i.WriteU16 (m_checksum);
    }
  */
/*
  i.WriteU8 (m_checksum);

  i.WriteHtonU32(m_RTXRSn);
  if (GetX())
  {
      i.WriteHtonU32(m_sequenceNumberLow);
  }
}

uint32_t
DccpHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_sourcePort = i.ReadNtohU16 ();
  m_destinationPort = i.ReadNtohU16 ();

  m_dataOffset = i.ReadU8();
  m_CCVal_CsCov = i.ReadU8();

  m_checksum = i.ReadU8 ();

  /*
  if (m_calcChecksum)
    {
      uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
      i = start;
      uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);

      m_goodChecksum = (checksum == 0);
    }
  */
/*
  m_RTXRSn = i.ReadNtohU32();
  if (GetX())
  {
      m_sequenceNumberLow = i.ReadNtohU32();
  }

  return GetSerializedSize ();
}

//------------------------------------------------------------------------------Getters
uint8_t
DccpHeader::GetDataOffset () const
{
  return m_dataOffset;
}

uint8_t
DccpHeader::GetCCVal () const
{
  return (m_CCVal_CsCov & 0xf0) >> 4;
}

uint8_t
DccpHeader::GetCsCov () const
{
  return (m_CCVal_CsCov & 0x0f);
}

uint8_t
DccpHeader::GetRes () const
{
  return (m_RTXRSn & 0xe0000000) >> 29;
}

uint8_t
DccpHeader::GetType () const
{
  return (m_RTXRSn & 0x1e000000) >> 25;
}

bool
DccpHeader::GetX () const
{
  return (m_RTXRSn & 0x01000000) >> 24;
}

uint8_t
DccpHeader::GetReserved () const
{
  return (m_RTXRSn & 0x00ff0000) >> 16;
}

SequenceNumber16
DccpHeader::GetSequenceNumberHigh () const
{
  return m_RTXRSn & 0x0000ffff;
}

SequenceNumber32
DccpHeader::GetSequenceNumberLow () const
{
  if (GetX())
    {
      return m_sequenceNumberLow;
    }
  else
    {
      return m_RTXRSn & 0x00ffffff;
    }
}

//------------------------------------------------------------------------------Setters
void
DccpHeader::SetDataOffset (uint8_t dataOffset)
{
  m_dataOffset = dataOffset;
}

void
DccpHeader::SetCCVal (uint8_t CCVal)
{
  uint8_t temp = m_CCVal_CsCov & 0x0f;
  m_CCVal_CsCov = (CCVal & 0x0f) << 4;
  m_CCVal_CsCov |= temp;
}

void
DccpHeader::SetCsCov (uint8_t CsCov)
{
  uint8_t temp = m_CCVal_CsCov & 0xf0;
  m_CCVal_CsCov = CsCov & 0x0f;
  m_CCVal_CsCov |= temp;
}

void
DccpHeader::SetRes (uint8_t Res)
{
  uint32_t temp = m_RTXRSn & 0x1fffffff;
  m_RTXRSn = (Res & 0x0e) << 29;
  m_RTXRSn |= temp;
}

void
DccpHeader::SetType (uint8_t Type)
{
  uint32_t temp = m_RTXRSn & 0xe1ffffff;
  m_RTXRSn = (Type & 0x0f) << 25;
  m_RTXRSn |= temp;
}

void
DccpHeader::SetX (bool X)
{
  uint32_t temp = m_RTXRSn & 0xfeffffff;
  m_RTXRSn = X << 24;
  m_RTXRSn |= temp;
}

void
DccpHeader::SetReserved (uint8_t Reserved)
{
  uint32_t temp = m_RTXRSn & 0xff00ffff;
  m_RTXRSn = (Reserved & 0xff) << 16;
  m_RTXRSn |= temp;
}

void
DccpHeader::SetSequenceNumberHigh (SequenceNumber16 SequenceNumberHigh)
{
  uint32_t temp = m_RTXRSn & 0xffff0000;
  m_RTXRSn = SequenceNumberHigh;
  m_RTXRSn |= temp;
}

void
DccpHeader::SetSequenceNumberLow (SequenceNumber32 SequenceNumberLow)
{
  if (GetX())
    {
      m_sequenceNumberLow = SequenceNumberLow;
    }
  else
    {
      uint32_t temp = m_RTXRSn & 0xff000000;
      m_RTXRSn = SequenceNumberLow & 0x00ffffff;
      m_RTXRSn |= temp;
    }
}
*/
} // namespace ns3
