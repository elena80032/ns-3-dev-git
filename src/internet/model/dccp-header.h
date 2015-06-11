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
  /*
  * This class has fields corresponding to those in a network DCCP generic header
  * as well as methods for serialization
  * to and deserialization from a byte buffer.
  */

class DccpHeader : public UdpHeader
{
public:

  /**
   * \brief Constructor
   */
  DccpHeader();

  /**
   * \brief Distructor
   */
  ~DccpHeader ();


  /**
   * \return Serialized Size
   */
  uint32_t GetSerializedSize (void) const;

  /**
   * \brief Serialize function
   */
  void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize function
   */
  uint32_t Deserialize (Buffer::Iterator start);

  /**
   * \param source the ip source to use in the underlying
   *        ip packet.
   * \param destination the ip destination to use in the
   *        underlying ip packet.
   * \param protocol the protocol number to use in the underlying
   *        ip packet.
   *
   * If you want to use udp checksums, you should call this
   * method prior to adding the header to a packet.
   */
  void InitializeChecksum (Address source,Address destination, uint8_t protocol);

  /**
   * \param source the ip source to use in the underlying
   *        ip packet.
   * \param destination the ip destination to use in the
   *        underlying ip packet.
   * \param protocol the protocol number to use in the underlying
   *        ip packet.
   *
   * If you want to use udp checksums, you should call this
   * method prior to adding the header to a packet.
   */
  void InitializeChecksum (Ipv4Address source, Ipv4Address destination, uint8_t protocol);

  /**
   * \param source the ip source to use in the underlying
   *        ip packet.
   * \param destination the ip destination to use in the
   *        underlying ip packet.
   * \param protocol the protocol number to use in the underlying
   *        ip packet.
   *
   * If you want to use udp checksums, you should call this
   * method prior to adding the header to a packet.
   */
  void InitializeChecksum (Ipv6Address source, Ipv6Address destination, uint8_t protocol);

  /**
   * \return if packet is corrupted
   */
  bool isCorrupted();

  /**
   * \return the value of DataOffset of this DccpHeader
   */
  uint8_t GetDataOffset () const;

  /**
   * \return the value of CCVal of this DccpHeader
   */
  uint8_t GetCCVal () const;

  /**
   * \return the value of ChecksumCoverage of this DccpHeader
   */
  uint8_t GetCsCov () const;

  /**
   * \return if checksum is ok for this DccpHeader
   */
  bool IsChecksumOk() const;

  /**
   * \return the value of Res of this DccpHeader
   */
  uint8_t GetRes () const;

  /**
   * \return the value of Type of this DccpHeader
   */
  uint8_t GetType () const;

  /**
   * \return the value of Extended Sequence Numbers of this DccpHeader
   */
  uint8_t GetX () const;

  /**
   * \return the value of Reserved of this DccpHeader
   */
  uint8_t GetReserved () const;

  /**
   * \return the value of the highest bit of Sequence Number of this DccpHeader
   */
  SequenceNumber16 GetSequenceNumberHigh () const;

  /**
   * \return the value of the lowest bit of Sequence Number of this DccpHeader
   */
  SequenceNumber32 GetSequenceNumberLow () const;

  /**
   * \return the value of Reserved field of Acknowledgement sub-header of this DccpHeader
   */
  uint16_t GetReservedAck () const; //Reserved ack subheader

  /**
   * \return the value of the highest bit of Acknowledgement Number of this DccpHeader
   */
  SequenceNumber16 GetAcknowloedgeNumberHigh () const;

  /**
   * \return the value of the lowest bit of Acknowledgement Number of this DccpHeader
   */
  SequenceNumber32 GetAcknowloedgeNumberLow () const;


  /**
   * \return the value of Service Code of this DccpHeader (DCCP_REQUEST & DCCP_RESPONSE)
   */
  uint32_t GetServiceCode() const;

  /**
   * \return the value of Reset Code of this DccpHeader (DCCP_RESET)
   */
  uint8_t GetResetCode() const;

  /**
   * \return the value of Data 1 of this DccpHeader (DCCP_RESET)
   */
  uint8_t GetData1() const;

  /**
   * \return the value of Data 2 of this DccpHeader (DCCP_RESET)
   */
  uint8_t GetData2() const;

  /**
   * \return the value of Data 3 of this DccpHeader (DCCP_RESET)
   */
  uint8_t GetData3() const;





  /**
   * \param dataOffset the Data Offset for this DccpHeader
   */
  void SetDataOffset (uint8_t dataOffset);

  /**
   * \param CCVal the CCVal for this DccpHeader
   */
  void SetCCVal (uint8_t CCVal);

  /**
   * \param CsCov the value of checksum coverage for this DccpHeader
   */
  void SetCsCov (uint8_t CsCov);

  /**
   * \param Res the value of res for this DccpHeader
   */
  void SetRes (uint8_t Res);

  /**
   * \param Type the Type of this DccpHeader
   */
  void SetType (uint8_t Type);

  /**
   * \param X the Extended Sequence Numbers  for this DccpHeader
   */
  void SetX (uint8_t  X);

  /**
   * \param Reserved the value of Reserved for this DccpHeader
   */
  void SetReserved (uint8_t Reserved);

  /**
   * \param SequenceNumberHigh the value of the highest bit of Sequence Number of this DccpHeader
   */
  void SetSequenceNumberHigh (SequenceNumber16 SequenceNumberHigh);

  /**
   * \param SequenceNumberLow the value of the lowest bit of Sequence Number of this DccpHeader
   */
  void SetSequenceNumberLow (SequenceNumber32 SequenceNumberLow);

  /**
   * \param ReservedAck the value of Reserved in Acknowledgement sub header for this DccpHeader
   */
  void SetReservedAck (uint16_t ReservedAck);

  /**
   * \param AcknowloedgeNumberHigh the value of the higher bit of Acknowlwdgement Number of  this DccpHeader
   */
  void SetAcknowloedgeNumberHigh (SequenceNumber16 AcknowloedgeNumberHigh);

  /**
   * \param AcknowloedgeNumberLow the value of the lowest bit of Acknowlwdgement Number of  this DccpHeader
   */
  void SetAcknowloedgeNumberLow (SequenceNumber32 AcknowloedgeNumberLow);

  /**
   * \param ServiceCode the value of Service Code of this DccpHeader (DCCP_REQUEST & DCCP_RESPONSE)
   */
  void SetServiceCode (uint32_t ServiceCode);

  /**
   * \param ResetCode the value of Reset Code of this DccpHeader (DCCP_RESET)
   */
  void SetResetCode (uint8_t ResetCode);

  /**
   * \param Data1 the value of Data 1 of this DccpHeader (DCCP_RESET)
   */
  void SetData1 (uint8_t Data1);

  /**
   * \param Data2 the value of Data 2 of this DccpHeader (DCCP_RESET)
   */
  void SetData2 (uint8_t Data2);

  /**
   * \param Data3 the value of Data 3 of this DccpHeader (DCCP_RESET)
   */
  void SetData3 (uint8_t Data3);



  /**
   * \param kind the kind ok option
   * \return  pointer to DccpOption
   */
  Ptr<DccpOption> GetOption (uint8_t kind) const;

  /**
   * \param kind the kind ok option
   * \return true if this DccpHeader has an option of kind kind, false otherwise
   */
  bool HasOption (uint8_t kind) const;

  /**
   * \param option a pointer to DccpOption
   * \return true if the option is added correctly, false otherwise
   */
  bool AppendOption (Ptr<DccpOption> option);

  /**
   * \return the length of the options
   */
  uint16_t GetOptionsLen();



  /**
   * DCCP PACKET TYPE
   */
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

  /**
   * @param size the size of the DccpPacket
   * @return the checksum of the IP PseudoHeader
   */
  uint16_t CalculateHeaderChecksum (uint16_t size) const;


  Address m_source;                                         //!< Source IP address
  Address m_destination;                                    //!< Destination IP address
  uint8_t m_protocol;                                       //!< Protocol number
  uint16_t m_checksum;                                      //!< Checksum value
  bool m_goodchecksum;                                      //!< Checksum flag
  uint8_t m_dataOffset;                                     //!< DataOffset
  uint8_t m_CCVal_CsCov;                                    //!< CCVal & CsCov fields
  uint32_t m_RTXRSn;                                        //!< Res & Type & X & { [ Reserved & Sequence Number High ] OR [ Sequence Number Low ] }
  SequenceNumber32 m_SequenceNumberLow;                     //!< Sequence Number Low
  uint32_t m_ReservedAcknwledgment;                         //!< { [ Reserved(16 bits) & Acknowledgement Number High ] OR [ Reserved(8 buts) & Acknowledgement Number Low  ] }
  SequenceNumber32 m_AcknowledgeLow;                        //!< Acknowledgement Number Low
  uint32_t m_ServiceCode;                                   //!< Service Code
  typedef std::list< Ptr<DccpOption> > DccpOptionList;      //!< List of DccpOption
  DccpOptionList m_options;                                 //!< List of DccpOption present in the header
  uint8_t m_optionsLen;                                     //!< Dccp options length.
  };
}
#endif // DCCPHEADER_H
