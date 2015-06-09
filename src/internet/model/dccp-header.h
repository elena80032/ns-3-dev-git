#ifndef DCCPHEADER_H
#define DCCPHEADER_H

#include "ns3/udp-header.h"
#include "ns3/header.h"
#include "ns3/sequence-number.h"
#include "dccp-option.h"
#include "dccp-option-pad.h"
#include "ns3/buffer.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"

namespace ns3 {

class DccpHeader : public UdpHeader
{
public:
  DccpHeader();
  ~DccpHeader ();


  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void InitializeChecksum (Address source,
                           Address destination,
                           uint8_t protocol);

  void InitializeChecksum (Ipv4Address source,
                           Ipv4Address destination,
                           uint8_t protocol);


  void InitializeChecksum (Ipv6Address source,
                           Ipv6Address destination,
                           uint8_t protocol);

  bool isCorrupted();

  //Getters
    uint8_t GetDataOffset () const;
    uint8_t GetCCVal () const;
    uint8_t GetCsCov () const;
    bool IsChecksumOk() const;
    uint8_t GetRes () const;
    uint8_t GetType () const;
    uint8_t GetX () const;
    uint8_t GetReserved () const;
    SequenceNumber16 GetSequenceNumberHigh () const;
    SequenceNumber32 GetSequenceNumberLow () const;
    uint16_t GetReservedAck () const; //Reserved ack subheader
    SequenceNumber16 GetAcknowloedgeNumberHigh () const;
    SequenceNumber32 GetAcknowloedgeNumberLow () const;
    uint32_t GetServiceCode() const;
    uint8_t GetResetCode() const;
    uint8_t GetData1() const;
    uint8_t GetData2() const;
    uint8_t GetData3() const;

  //Setters
    void SetDataOffset (uint8_t dataOffset);
    void SetCCVal (uint8_t CCVal);
    void SetCsCov (uint8_t CsCov);
    void SetRes (uint8_t Res);
    void SetType (uint8_t Type);
    void SetX (uint8_t  X);
    void SetReserved (uint8_t Reserved);
    void SetSequenceNumberHigh (SequenceNumber16 SequenceNumberHigh);
    void SetSequenceNumberLow (SequenceNumber32 SequenceNumberLow);
    void SetReservedAck (uint16_t ReservedAck);
    void SetAcknowloedgeNumberHigh (SequenceNumber16 AcknowloedgeNumberHigh);
    void SetAcknowloedgeNumberLow (SequenceNumber32 AcknowloedgeNumberLow);
    void SetServiceCode (uint32_t ServiceCode);
    void SetResetCode (uint8_t ResetCode);
    void SetData1 (uint8_t Data1);
    void SetData2 (uint8_t Data2);
    void SetData3 (uint8_t Data3);

  //Options
    Ptr<DccpOption> GetOption (uint8_t kind) const;
    bool HasOption (uint8_t kind) const;
    bool AppendOption (Ptr<DccpOption> option);

    uint16_t GetOptionsLen();

   typedef enum
   {
     REQUEST = 0,  
     RESPONSE  = 1,  
     DATA  = 2,  
     ACK  = 3,   
     DATAACK  = 4,   
     CLOSEREQ  = 5,  
     CLOSE  = 6,
     RESET  = 7, 
     SYNC  = 8,
     SYNCACK = 9,
   } DCCP_packet_type;

private:
    uint16_t CalculateHeaderChecksum (uint16_t size) const;
    Address m_source;           //!< Source IP address
    Address m_destination;      //!< Destination IP address
    uint8_t m_protocol;         //!< Protocol number
    uint16_t m_checksum;
    bool m_goodchecksum;
    uint8_t m_dataOffset;
    uint8_t m_CCVal_CsCov;
    uint32_t m_RTXRSn;
    SequenceNumber32 m_SequenceNumberLow;
    uint32_t m_ReservedAcknwledgment; 
    SequenceNumber32 m_AcknowledgeLow;
    uint32_t m_ServiceCode;
    typedef std::list< Ptr<DccpOption> > DccpOptionList; //!< List of DccpOption
    DccpOptionList m_options; //!< DccpOption present in the header
    uint8_t m_optionsLen; //!< Dccp options length.
  };
}
#endif // DCCPHEADER_H
