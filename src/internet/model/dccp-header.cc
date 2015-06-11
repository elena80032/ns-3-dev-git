#include "dccp-header.h"
#include "ns3/log.h"
#include "ns3/type-id.h"
#include <stdint.h>
#include <iostream>

namespace ns3 {

  NS_LOG_COMPONENT_DEFINE ("DccpHeader");

  NS_OBJECT_ENSURE_REGISTERED (DccpHeader);

DccpHeader::DccpHeader()
  :
    m_source(Address()),
    m_destination(Address()),
    m_protocol(0),
    m_checksum(0),
    m_goodchecksum(false),
    m_dataOffset(0),
    m_CCVal_CsCov(0),
    m_RTXRSn(0),
    m_SequenceNumberLow(0),
    m_ReservedAcknwledgment(0),
    m_AcknowledgeLow(0),
    m_ServiceCode(0),
    m_optionsLen (0)
{
}

DccpHeader::~DccpHeader ()
{
}

uint32_t
DccpHeader::GetSerializedSize (void) const
{
  if (GetX())
    {
      if(GetType () != REQUEST && GetType () != DATA)
        {
          // Full generic header and full acknowledgment subheader
          return 24+m_optionsLen;
        }
      else
        {
          // Full generic header
          return 16+m_optionsLen;
        }
    }
  else
    {
      if(GetType () != REQUEST && GetType () != DATA)
        {
          // Partial generic header and partial acknowledgment subheader
          return 16+m_optionsLen;
        }
      else
        {
          // Partial generic header
          return 12+m_optionsLen;
        }
    }
}

void
DccpHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU16 (GetSourcePort());
  i.WriteHtonU16 (GetDestinationPort());
  i.WriteU8(m_dataOffset);
  i.WriteU8(m_CCVal_CsCov);

  i.WriteU16 (0); //checksum disabled

  i.WriteHtonU32(m_RTXRSn);
  if (GetX())
    {
      i.WriteHtonU32(m_SequenceNumberLow.GetValue ());
    }

  if(GetType () != REQUEST && GetType () != DATA)
    {
      i.WriteHtonU32(m_ReservedAcknwledgment);
      if (GetX())
        {
          i.WriteHtonU32(m_AcknowledgeLow.GetValue ());
        }
    }

  if (GetDataOffset()*4 > GetSerializedSize())
  {
      DccpOptionList::const_iterator op;
      uint32_t optionLen=0;
      int j = 0;
      for (op = m_options.begin (); op != m_options.end (); ++op)
        {
          j++;
          (*op)->Serialize (i);
          optionLen += (*op)->GetSerializedSize ();
          i.Next ((*op)->GetSerializedSize ());
        }

      // padding to word alignment; add or not pad values (they are the same)
      while (optionLen % 4)
      {
          DccpOptionPadding pad;
          pad.Serialize(i);
          ++optionLen;

      }
  }

  uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
  i = start;
  uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);
  i = start;
  i.Next (6);
  i.WriteU16 (checksum);
}

uint32_t
DccpHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetSourcePort(i.ReadNtohU16 ());
  SetDestinationPort(i.ReadNtohU16 ());

  m_dataOffset = i.ReadU8();
  m_CCVal_CsCov = i.ReadU8();

  m_checksum = i.ReadU16 ();

  m_RTXRSn = i.ReadNtohU32();
  if (GetX())
  {
      m_SequenceNumberLow = SequenceNumber32(i.ReadNtohU32());
  }

  if(GetType () != REQUEST && GetType () != DATA)
    {
      m_ReservedAcknwledgment = i.ReadNtohU32();
      if (GetX())
        {
          m_AcknowledgeLow = SequenceNumber32(i.ReadNtohU32());
        }
    }
  // Deserialize options if they exist
  m_options.clear ();

  uint8_t optionLen = (GetSerializedSize()-m_dataOffset*4);
  m_optionsLen = optionLen;

  while (optionLen)
    {
      uint8_t kind = i.PeekU8 ();
      Ptr<DccpOption> op;
      uint32_t optionSize;
      if (DccpOption::IsKindKnown (kind))
        {
          op = DccpOption::CreateOption (kind);
        }
      else
        {
          op = DccpOption::CreateOption (DccpOption::UNKNOWN);
          NS_LOG_WARN ("Option kind " << static_cast<int> (kind) << " unknown, skipping.");
        }
      optionSize = op->Deserialize (i);
      if (optionSize != op->GetSerializedSize ())
        {
          NS_LOG_ERROR ("Option did not deserialize correctly");
          break;
        }
      if (optionLen >= optionSize)
        {
          optionLen -= optionSize;
          i.Next (optionSize);
          m_options.push_back (op);
        }
      else
        {
          NS_LOG_ERROR ("Option exceeds DCCP option space; option discarded");
          break;
        }
    }

   uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
   i = start;
   uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);
   m_goodchecksum = (checksum == 0);

  return GetSerializedSize () ;
}

uint16_t
DccpHeader::CalculateHeaderChecksum (uint16_t size) const
{
  /* Buffer size must be at least as large as the largest IP pseudo-header */
  /* Src address            16 bytes (more generally, Address::MAX_SIZE)   */
  /* Dst address            16 bytes (more generally, Address::MAX_SIZE)   */
  /* Upper layer pkt len    4 bytes                                        */
  /* Zero                   3 bytes                                        */
  /* Next header            1 byte                                         */

  uint32_t maxHdrSz = (2 * Address::MAX_SIZE) + 8;
  Buffer buf = Buffer (maxHdrSz);
  buf.AddAtStart (maxHdrSz);
  Buffer::Iterator it = buf.Begin ();
  uint32_t hdrSize = 0;

  WriteTo (it, m_source);
  WriteTo (it, m_destination);
  if (Ipv4Address::IsMatchingType (m_source))
    {
      it.WriteU8 (0); /* protocol */
      it.WriteU8 (m_protocol); /* protocol */
      it.WriteU8 (size >> 8); /* length */
      it.WriteU8 (size & 0xff); /* length */
      hdrSize = 12;
    }
  else
    {
      it.WriteU16 (0);
      it.WriteU8 (size >> 8); /* length */
      it.WriteU8 (size & 0xff); /* length */
      it.WriteU16 (0);
      it.WriteU8 (0);
      it.WriteU8 (m_protocol); /* protocol */
      hdrSize = 40;
    }

  it = buf.Begin ();
  /* we don't CompleteChecksum ( ~ ) now */
  return ~(it.CalculateIpChecksum (hdrSize));
}
void
DccpHeader::InitializeChecksum (Address source,
                               Address destination,
                               uint8_t protocol)
{
  m_source = source;
  m_destination = destination;
  m_protocol = protocol;
}
void
DccpHeader::InitializeChecksum (Ipv4Address source,
                               Ipv4Address destination,
                               uint8_t protocol)
{
  m_source = source;
  m_destination = destination;
  m_protocol = protocol;
}
void
DccpHeader::InitializeChecksum (Ipv6Address source,
                               Ipv6Address destination,
                               uint8_t protocol)
{
  m_source = source;
  m_destination = destination;
  m_protocol = protocol;
}


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

bool
DccpHeader::IsChecksumOk (void) const
{
  return m_goodchecksum;
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

uint8_t
DccpHeader::GetX () const
{
  return (m_RTXRSn & 0x01000000) >> 24;
}

uint8_t
DccpHeader::GetReserved () const
{
  if (GetX())
    {
    return (m_RTXRSn & 0x00ff0000) >> 16;
    }
  else
    {
    return uint8_t(0);
    }
}

SequenceNumber16
DccpHeader::GetSequenceNumberHigh () const
{
  if(GetX())
    {
    return SequenceNumber16(m_RTXRSn & 0x0000ffff);
    }
  else
    {
      return SequenceNumber16(0);
    }
}

SequenceNumber32
DccpHeader::GetSequenceNumberLow () const
{
  if (GetX())
    {
      return m_SequenceNumberLow;
    }
  else
    {
      return SequenceNumber32(m_RTXRSn & 0x00ffffff);
    }
}

uint16_t
DccpHeader::GetReservedAck () const
{
  if (GetX())
    {
      return (m_ReservedAcknwledgment & 0xffff0000) >> 16;
    }
  else
    {
      return (m_ReservedAcknwledgment & 0xff000000) >> 24;
    }
}

SequenceNumber16
DccpHeader::GetAcknowloedgeNumberHigh () const
{
  if (GetX())
    {
      return SequenceNumber16((m_ReservedAcknwledgment & 0x0000ffff));
    }
  else
    {
      return SequenceNumber16(0);
    }
}

SequenceNumber32
DccpHeader::GetAcknowloedgeNumberLow () const
{
  if (GetX())
  {
      return m_AcknowledgeLow;
  }
  else
  {
      return SequenceNumber32((m_ReservedAcknwledgment & 0x00ffffff));
  }
}

uint32_t 
DccpHeader::GetServiceCode() const
{
 if (GetType() == REQUEST || GetType() == RESPONSE)
  {
    return m_ServiceCode;
  }
 else 
  {
    return 0;
  }
}

uint8_t 
DccpHeader::GetResetCode() const
{
  if (GetType() == RESET)
   {
    return (m_ServiceCode & 0xff000000) >> 24;
   }
  else 
   {
    return 0;
   }
}

uint8_t
DccpHeader::GetData1() const
{
  if (GetType() == RESET)
   {
    return (m_ServiceCode & 0x00ff0000) >> 16;
   }
  else 
   {
    return 0;
   }
}

uint8_t
DccpHeader::GetData2() const
{
  if (GetType() == RESET)
   {
    return (m_ServiceCode & 0x0000ff00) >> 8;
   }
  else 
   {
    return 0;
   }
}

uint8_t 
DccpHeader::GetData3() const
{  
   if (GetType() == RESET)
   {
    return (m_ServiceCode & 0x000000ff);
   }
  else 
   {
    return 0;
   }
}


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
  m_RTXRSn = (Res & 0x07) << 29;
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
DccpHeader::SetX (uint8_t X)
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
  m_RTXRSn = SequenceNumberHigh.GetValue();
  m_RTXRSn |= temp;
}

void
DccpHeader::SetSequenceNumberLow (SequenceNumber32 SequenceNumberLow)
{
  if (GetX())
    {
      m_SequenceNumberLow = SequenceNumberLow;
    }
  else
    {
      uint32_t temp = m_RTXRSn & 0xff000000;
      m_RTXRSn = SequenceNumberLow.GetValue() & 0x00ffffff;
      m_RTXRSn |= temp;
    }
}

void
DccpHeader::SetReservedAck (uint16_t ReservedAck)
{
  if (GetX())
    {
      uint32_t temp = m_ReservedAcknwledgment & 0x0000ffff;
      m_ReservedAcknwledgment = (ReservedAck << 16);
      m_ReservedAcknwledgment |= temp;
    }
  else
    {
      uint32_t temp = m_ReservedAcknwledgment & 0x00ffffff;
      m_ReservedAcknwledgment = (ReservedAck & 0x00ff) << 24;
      m_ReservedAcknwledgment |= temp;
    }
}

void
DccpHeader::SetAcknowloedgeNumberHigh (SequenceNumber16 AcknowloedgeNumberHigh)
{
  if (GetX())
  {
      uint32_t temp = m_ReservedAcknwledgment & 0xffff0000;
      m_ReservedAcknwledgment = AcknowloedgeNumberHigh.GetValue();
      m_ReservedAcknwledgment |= temp;
  }
}

void
DccpHeader::SetAcknowloedgeNumberLow (SequenceNumber32 AcknowloedgeNumberLow)
{
  if (GetX())
    {
      m_AcknowledgeLow = AcknowloedgeNumberLow;
    }
  else
    {
      uint32_t temp = m_ReservedAcknwledgment & 0xff000000;
      m_ReservedAcknwledgment = (AcknowloedgeNumberLow.GetValue() & 0x00ffffff);
      m_ReservedAcknwledgment |= temp;
    }
}

void
DccpHeader::SetServiceCode (uint32_t ServiceCode)
{
  m_ServiceCode = ServiceCode;
}

void
DccpHeader::SetResetCode (uint8_t ResetCode)
{
  uint32_t temp = m_ServiceCode & 0x00ffffff;
  m_ServiceCode = ResetCode << 24;
  m_ServiceCode |= temp;
}

void
DccpHeader::SetData1 (uint8_t Data1)
{
  uint32_t temp = m_ServiceCode & 0xff00ffff;
  m_ServiceCode = Data1 << 16;
  m_ServiceCode |= temp;
}

void
DccpHeader::SetData2 (uint8_t Data2)
{
  uint32_t temp = m_ServiceCode & 0xffff00ff;
  m_ServiceCode = Data2 << 8;
  m_ServiceCode |= temp;
}

void
DccpHeader::SetData3 (uint8_t Data3)
{
  uint32_t temp = m_ServiceCode & 0xffffff00;
  m_ServiceCode = Data3;
  m_ServiceCode |= temp;
}

// Options handling
bool
DccpHeader::AppendOption (Ptr<DccpOption> option)
{
  if (!DccpOption::IsKindKnown (option->GetKind ()))
    {
      NS_LOG_WARN ("The option kind " << static_cast<int> (option->GetKind ()) << " is unknown");
      return false;
    }
  m_options.push_back (option);
  m_optionsLen += option->GetSerializedSize ();
  return true;
}

Ptr<DccpOption>
DccpHeader::GetOption (uint8_t kind) const
{
  DccpOptionList::const_iterator i;
  for (i = m_options.begin (); i != m_options.end (); ++i)
    {
      if ((*i)->GetKind () == kind)
        {
          return (*i);
        }
    }
  return 0;
}

bool
DccpHeader::HasOption (uint8_t kind) const
{
  DccpOptionList::const_iterator i;
  for (i = m_options.begin (); i != m_options.end (); ++i)
    {
      if ((*i)->GetKind () == kind)
        {
          return true;
        }
    }

  return false;
}

uint16_t
DccpHeader::GetOptionsLen()
{
  return m_dataOffset* 4 - GetSerializedSize();
}

// check if the couple X and type is valid
bool
DccpHeader::isCorrupted()
{
  switch(GetType())
    {
    case 0://REQUEST
    case 1://RESPONSE
    case 5://CLOSE_REQ
    case 6://CLOSE
    case 7://RESET
    case 8://SYNC
    case 9://SYNC_ACK
      if (GetX()){
          return false;
        }
      else{
          return true;
        }
      break;
    default:
      return false;
      break;
    }
}

} // namespace ns3
