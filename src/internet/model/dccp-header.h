#ifndef DCCPHEADER_H
#define DCCPHEADER_H

#include "ns3/udp-header.h"
#include "ns3/header.h"
#include "ns3/sequence-number.h"

namespace ns3 {

class DccpHeader : public UdpHeader
{
public:
  DccpHeader();
  ~DccpHeader ();

  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

  //Getters
    uint8_t GetDataOffset () const;
    uint8_t GetCCVal () const;
    uint8_t GetCsCov () const;
    uint8_t GetRes () const;
    uint8_t GetType () const;
    bool GetX () const;
    uint8_t GetReserved () const;
    SequenceNumber16 GetSequenceNumberHigh () const;
    SequenceNumber32 GetSequenceNumberLow () const;

  //Setters
    void SetDataOffset (uint8_t dataOffset);
    void SetCCVal (uint8_t CCVal);
    void SetCsCov (uint8_t CsCov);
    void SetRes (uint8_t Res);
    void SetType (uint8_t Type);
    void SetX (bool X);
    void SetReserved (uint8_t Reserved);
    void SetSequenceNumberHigh (SequenceNumber16 SequenceNumberHigh);
    void SetSequenceNumberLow (SequenceNumber32 SequenceNumberLow);

private:
    uint16_t m_sourcePort;      //!< Source port
    uint16_t m_destinationPort; //!< Destination port

    uint8_t m_dataOffset;
    uint8_t m_CCVal_CsCov; //!< CCVal and CsCov
    uint32_t m_RTXRSn; //!< if (X == 1) Res + Type + X + Reserved + Sequence number high if (X == 0) Res + Type + X + Sequence number
    SequenceNumber32 m_sequenceNumberLow;  //!< Sequence number Low bits
  };
}
#endif // DCCPHEADER_H
