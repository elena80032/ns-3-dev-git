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
#include "tcp-hstcp-mw.h"
#include "ns3/log.h"
#include "ns3/cc-l45-protocol.h"


NS_LOG_COMPONENT_DEFINE ("TcpHighSpeedMw");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpHighSpeedMw);

TypeId
TcpHighSpeedMw::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpHighSpeedMw")
    .SetParent<TcpHighSpeed> ()
    .AddConstructor<TcpHighSpeedMw> ()
  ;

  return tid;
}

TcpHighSpeedMw::TcpHighSpeedMw () : TcpHighSpeed ()
{
  m_availableBw = 0;
  m_minRtt = Time::Max ();
}

void
TcpHighSpeedMw::CloseAndNotify ()
{
  NS_LOG_FUNCTION_NOARGS ();

  TcpHighSpeed::CloseAndNotify ();

  Ptr<CCL45Protocol> tcp = GetMwProtocol ();
  tcp->NotifyConnectionClosed (this);
}

Ptr<CCL45Protocol>
TcpHighSpeedMw::GetMwProtocol ()
{
  Ptr<CCL45Protocol> tcp = DynamicCast<CCL45Protocol> (m_tcp);

  if (tcp == 0)
    {
      NS_FATAL_ERROR ("No middleware protocol");
    }

  return tcp;
}

void
TcpHighSpeedMw::NewAck (SequenceNumber32 const& seq)
{
  Time rtt = m_rtt->GetEstimate();

  if (m_minRtt > rtt)
    m_minRtt = rtt;

  m_ssThresh = m_availableBw * m_minRtt.GetSeconds ();

  TcpHighSpeed::NewAck (seq);

  if (m_cWnd > m_ssThresh)
    {
      m_cWnd = m_ssThresh;
    }

  SendPendingData (m_connected);
}

int
TcpHighSpeedMw::Connect (const Address &address)
{
  int res = TcpHighSpeed::Connect (address);
  Ptr<CCL45Protocol> tcp = GetMwProtocol ();
  tcp->NotifyConnectionOpened (this);

  return res;
}

void TcpHighSpeedMw::SetBandwidth (uint32_t b)
{
  NS_LOG_FUNCTION (this << b);

  Time rtt = m_rtt->GetEstimate();

  uint32_t oldBw = m_availableBw;
  m_availableBw = b;

  m_ssThresh = m_availableBw * rtt.GetSeconds ();

  if (oldBw > b) {
    m_cWnd = m_ssThresh / 2;
  }
}

bool
TcpHighSpeedMw::SendPendingData (bool withAck)
{
  bool res = TcpHighSpeed::SendPendingData (withAck);

  if (m_state > 4)
    {
      Ptr<CCL45Protocol> tcp = GetMwProtocol ();
      tcp->NotifyConnectionClosed (this);
    }

  return res;
}

} // namespace ns3
