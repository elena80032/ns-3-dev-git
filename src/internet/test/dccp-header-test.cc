/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Natale Patriciello <natale.patriciello@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "ns3/test.h"
#include "ns3/core-module.h"
#include "ns3/dccp-header.h"
#include "ns3/buffer.h"
#include "ns3/private/tcp-option-rfc793.h"

namespace ns3 {

class DccpHeaderGetSetTestCase : public TestCase
{
public:
  DccpHeaderGetSetTestCase ( uint16_t sourcePort, uint16_t destinationPort,
                            uint8_t dataOffset, uint8_t CCVal, uint8_t CsCov, uint8_t Res, uint8_t Type, uint8_t X, uint8_t Reserved,
                            SequenceNumber16 SequenceNumberHigh, SequenceNumber32 SequenceNumberLow,uint16_t ReservedAck, SequenceNumber16 AcknowledgeHigh,
                            SequenceNumber32 AcknowledgeLow, std::string name);
protected:
  uint16_t m_sourcePort;
  uint16_t m_destinationPort;
  uint8_t m_dataOffset;
  uint8_t m_cCVal;
  uint8_t m_csCov;
  uint8_t m_res;
  uint8_t m_type;
  uint8_t m_x;
  uint8_t m_reserved;
  SequenceNumber16 m_sequenceNumberHigh;
  SequenceNumber32 m_sequenceNumberLow;
  uint16_t m_reservedAck;
  SequenceNumber16 m_acknowledgeHigh;
  SequenceNumber32 m_acknowledgeLow;

private:
  virtual void DoRun (void);
  virtual void DoTeardown (void);

  DccpHeader m_header;
  Buffer m_buffer;
};

DccpHeaderGetSetTestCase::DccpHeaderGetSetTestCase
(uint16_t sourcePort, uint16_t destinationPort,
 uint8_t dataOffset, uint8_t CCVal, uint8_t CsCov, uint8_t Res, uint8_t Type, uint8_t X, uint8_t Reserved,
 SequenceNumber16 SequenceNumberHigh, SequenceNumber32 SequenceNumberLow,uint16_t ReservedAck, SequenceNumber16 AcknowledgeHigh,
 SequenceNumber32 AcknowledgeLow, std::string name) : TestCase (name)
{
  m_sourcePort = sourcePort;
  m_destinationPort = destinationPort;
  
  m_cCVal = CCVal;
  m_csCov = CsCov;
  m_res = Res;
  std::cout << "type " << int(Type) << std::endl;
  m_type = Type;
  std::cout << "X " << int(X) << std::endl;
  m_x = X;
  m_reserved = Reserved;
  m_sequenceNumberHigh = SequenceNumberHigh;
  m_sequenceNumberLow = SequenceNumberLow;
  std::cout << "ReservedAck " << int(ReservedAck) << std::endl;
  m_reservedAck = ReservedAck;
  std::cout << "AcknowledgeHigh " << AcknowledgeHigh << std::endl;
  m_acknowledgeHigh = AcknowledgeHigh;
  std::cout << "AcknowledgeLow " << AcknowledgeLow << std::endl;
  m_acknowledgeLow = AcknowledgeLow;


  m_header.SetSourcePort (m_sourcePort);
  m_header.SetDestinationPort (m_destinationPort);
  
  m_header.SetCCVal(m_cCVal);
  m_header.SetCsCov (m_csCov);
  m_header.SetRes (m_res);
  m_header.SetType (m_type);
  m_header.SetX(m_x);
  m_header.SetReserved(m_reserved);
  m_header.SetSequenceNumberHigh(m_sequenceNumberHigh);
  m_header.SetSequenceNumberLow(m_sequenceNumberLow);
  m_header.SetReservedAck(m_reservedAck);
  m_header.SetAcknowloedgeNumberHigh(m_acknowledgeHigh);
  m_header.SetAcknowloedgeNumberLow(m_acknowledgeLow);


//setting data offset ad hoc
  m_dataOffset = m_header.GetSerializedSize()/4;
  m_header.SetDataOffset (m_dataOffset);
  std::cout << "DataOffset " << int(dataOffset) << std::endl;
  std::cout << "Size " << int(m_header.GetSerializedSize()) << std::endl << std::endl << std::endl << std::endl;

  //TODO
  //NS_TEST_ASSERT_MSG_EQ (m_header.GetLength (), 5, "TcpHeader without option is not 5 word");

  m_buffer.AddAtStart (m_header.GetSerializedSize ());
  m_header.Serialize (m_buffer.Begin ());
}

void DccpHeaderGetSetTestCase::DoRun (void)
{

  NS_TEST_ASSERT_MSG_EQ (m_sourcePort, m_header.GetSourcePort (),"Different source port found");

  NS_TEST_ASSERT_MSG_EQ (m_destinationPort, m_header.GetDestinationPort (), "Different destination port found");

  NS_TEST_ASSERT_MSG_EQ (m_dataOffset, m_header.GetDataOffset (), "Different data offset found");

  NS_TEST_ASSERT_MSG_EQ (m_cCVal, m_header.GetCCVal (), "Different ccval found");

  NS_TEST_ASSERT_MSG_EQ (m_csCov, m_header.GetCsCov (), "Different cscov found");

  NS_TEST_ASSERT_MSG_EQ (m_res, m_header.GetRes (), "Different res found");

  NS_TEST_ASSERT_MSG_EQ (m_type, m_header.GetType (),"Different type found");

  NS_TEST_ASSERT_MSG_EQ (m_x, m_header.GetX (), "Different X found");

  NS_TEST_ASSERT_MSG_EQ (m_reserved, m_header.GetReserved (), "Different reserved found");

  NS_TEST_ASSERT_MSG_EQ (m_sequenceNumberHigh , m_header.GetSequenceNumberHigh (),"Different sequenceNumberHigh found");

  NS_TEST_ASSERT_MSG_EQ (m_sequenceNumberLow, m_header.GetSequenceNumberLow (), "Different sequenceNumberLow found");

  NS_TEST_ASSERT_MSG_EQ (m_reservedAck, m_header.GetReservedAck (), "Different reservedAck found");

  NS_TEST_ASSERT_MSG_EQ (m_acknowledgeHigh, m_header.GetAcknowloedgeNumberHigh (), "Different AcknowledgeHigh found");

  NS_TEST_ASSERT_MSG_EQ (m_acknowledgeLow, m_header.GetAcknowloedgeNumberLow (), "Different AcknowledgeLow found");

  //std::cout << "Checksum Ok SERIALIZE " << m_header.IsChecksumOk() << std::endl;
  //NS_TEST_ASSERT_MSG_EQ(true, m_header.IsChecksumOk() , "Different Checksum found in deserialized header");

  std::cout << "Options Len " << int(m_header.GetOptionsLen()) << std::endl;
  std::cout << "dataOffset - serializedSize " << int(m_dataOffset* 4 - m_header.GetSerializedSize()) << std::endl;
  NS_TEST_ASSERT_MSG_EQ (m_dataOffset* 4 - m_header.GetSerializedSize(), m_header.GetOptionsLen(), "Different Options Length");



  DccpHeader copyHeader;

  copyHeader.Deserialize (m_buffer.Begin ());

  NS_TEST_ASSERT_MSG_EQ (m_sourcePort, copyHeader.GetSourcePort (), "Different source port found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_destinationPort, copyHeader.GetDestinationPort (), "Different destination port found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_dataOffset, copyHeader.GetDataOffset (), "Different data offset found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_cCVal, copyHeader.GetCCVal (), "Different ccval found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_csCov, copyHeader.GetCsCov (), "Different cscov found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_res, copyHeader.GetRes (), "Different res found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_type, copyHeader.GetType (), "Different type found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_x, copyHeader.GetX (), "Different X found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_reserved, copyHeader.GetReserved (), "Different reserved found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_sequenceNumberHigh , copyHeader.GetSequenceNumberHigh (), "Different sequenceNumberHigh found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_sequenceNumberLow, copyHeader.GetSequenceNumberLow (), "Different sequenceNumberLow found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_reservedAck, copyHeader.GetReservedAck (), "Different reservedAck found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_acknowledgeHigh, copyHeader.GetAcknowloedgeNumberHigh (), "Different AcknowledgeHigh found in deserialized header");

  NS_TEST_ASSERT_MSG_EQ (m_acknowledgeLow, copyHeader.GetAcknowloedgeNumberLow (), "Different AcknowledgeLow found in deserialized header");

  //std::cout << "Checksum  Ok DESERIALIZE " << copyHeader.IsChecksumOk() << std::endl;
  //NS_TEST_ASSERT_MSG_EQ(true, copyHeader.IsChecksumOk() , "Different Checksum found in deserialized header");



  std::cout << "Options Len " << int(copyHeader.GetOptionsLen()) << std::endl;
  std::cout << "dataOffset - serializedSize " << int(m_dataOffset* 4 - m_header.GetSerializedSize()) << std::endl;
  NS_TEST_ASSERT_MSG_EQ (copyHeader.GetDataOffset()* 4 - copyHeader.GetSerializedSize(), m_header.GetOptionsLen(), "Different Options Length");

}

void DccpHeaderGetSetTestCase::DoTeardown (void)
{

}

#define GET_RANDOM_UINT32(RandomVariable) \
  static_cast<uint32_t> (RandomVariable->GetInteger (0, UINT32_MAX))

#define GET_RANDOM_UINT24(RandomVariable) \
  static_cast<uint32_t> (RandomVariable->GetInteger (0, UINT32_MAX >> 8))

#define GET_RANDOM_UINT16(RandomVariable) \
  static_cast<uint16_t> (RandomVariable->GetInteger (0, UINT16_MAX))

#define GET_RANDOM_UINT8(RandomVariable) \
  static_cast<uint8_t> (RandomVariable->GetInteger (0, UINT8_MAX))

#define GET_RANDOM_UINT6(RandomVariable) \
  static_cast<uint8_t> (RandomVariable->GetInteger (0, UINT8_MAX >> 2))

#define GET_RANDOM_UINT4(RandomVariable) \
  static_cast<uint8_t> (RandomVariable->GetInteger (0, UINT8_MAX >> 4))

#define GET_RANDOM_UINT3(RandomVariable) \
  static_cast<uint8_t> (RandomVariable->GetInteger (0, UINT8_MAX >> 5))

#define GET_RANDOM_UINT1(RandomVariable) \
  static_cast<uint8_t> (RandomVariable->GetInteger (0, UINT8_MAX >> 7))

static class DccpHeaderTestSuite : public TestSuite
{
public:
  DccpHeaderTestSuite ()
    : TestSuite ("dccp-header", UNIT)
  {
    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();

    for (uint32_t i = 0; i < 2; ++i)
      {
        std::stringstream ss;
        ss << i;

        uint8_t xRandom = GET_RANDOM_UINT1(x);
        uint8_t typeRandom = GET_RANDOM_UINT4(x);

        if(typeRandom != DccpHeader::REQUEST && typeRandom != DccpHeader::DATA)
          {
            AddTestCase (new DccpHeaderGetSetTestCase (GET_RANDOM_UINT16 (x),
                                                   GET_RANDOM_UINT16 (x),
                                                   //GET_RANDOM_UINT8(x),
                                                   0,
                                                   GET_RANDOM_UINT4(x),
                                                   GET_RANDOM_UINT4(x),
                                                   GET_RANDOM_UINT3(x),
                                                   typeRandom,
                                                   xRandom,
                                                   xRandom ? GET_RANDOM_UINT8(x) : 0,
                                                   xRandom ? SequenceNumber16 (GET_RANDOM_UINT16 (x)) : SequenceNumber16(0),
                                                   xRandom ? SequenceNumber32 (GET_RANDOM_UINT32 (x)) : SequenceNumber32 (GET_RANDOM_UINT24 (x)),
                                                   xRandom ? GET_RANDOM_UINT16(x) : GET_RANDOM_UINT8(x),
                                                   xRandom ? SequenceNumber16 (GET_RANDOM_UINT16 (x)) : SequenceNumber16(0),
                                                   xRandom ? SequenceNumber32 (GET_RANDOM_UINT32 (x)) : SequenceNumber32 (GET_RANDOM_UINT24 (x)),
                                                   "Case number " + ss.str ()),
                     TestCase::QUICK);
          }
        else
          {
            AddTestCase (new DccpHeaderGetSetTestCase (GET_RANDOM_UINT16 (x),
                                                   GET_RANDOM_UINT16 (x),
                                                   //GET_RANDOM_UINT8(x),
                                                   0,
                                                   GET_RANDOM_UINT4(x),
                                                   GET_RANDOM_UINT4(x),
                                                   GET_RANDOM_UINT3(x),
                                                   typeRandom,
                                                   xRandom,
                                                   xRandom ? GET_RANDOM_UINT8(x) : 0,
                                                   xRandom ? SequenceNumber16 (GET_RANDOM_UINT16 (x)) : SequenceNumber16(0),
                                                   xRandom ? SequenceNumber32 (GET_RANDOM_UINT32 (x)) : SequenceNumber32 (GET_RANDOM_UINT24 (x)),
                                                   uint16_t(0),
                                                   SequenceNumber16(0) ,
                                                   SequenceNumber32(0),
                                                   "Case number " + ss.str ()),
                     TestCase::QUICK);
          }
      }
  }

} g_DccpHeaderTestSuite;

} // namespace ns3
