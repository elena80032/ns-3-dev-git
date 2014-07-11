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

#define NS_LOG_APPEND_CONTEXT \
  if (m_node) \
    { std::clog << Simulator::Now ().GetSeconds () << \
        " [node " << m_node->GetId () << "] "; }

#include "tcp-hybla.h"
#include "ns3/log.h"
#include "ns3/node.h"


NS_LOG_COMPONENT_DEFINE ("TcpHybla");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpHybla);

TypeId
TcpHybla::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpHybla")
    .SetParent<TcpNewReno> ()
    .AddConstructor<TcpHybla> ()
    .AddAttribute ("RRTT", "Reference RTT",
                   TimeValue (MilliSeconds (50)),
                   MakeTimeAccessor (&TcpHybla::m_rRtt),
                   MakeTimeChecker ())
    //.AddAttribute ("LimitedTransmit", "Enable limited transmit",
    //                BooleanValue (false),
    //                MakeBooleanAccessor (&TcpHybla::m_limitedTx),
    //                MakeBooleanChecker ())
  ;
  return tid;
}

TcpHybla::TcpHybla () : TcpNewReno ()
{
  m_minRtt = Time::Max ();

  m_rho = 1.0;
}

void
TcpHybla::InitializeCwnd ()
{
  NS_LOG_FUNCTION (this);

  /* 1st Rho measurement based on initial srtt */
  RecalcParam ();

  /* set minimum rtt as this is the 1st ever seen */
  m_minRtt = m_rtt->GetCurrentEstimate ();

  TcpNewReno::InitializeCwnd ();

}

void
TcpHybla::SetSSThresh (uint32_t threshold)
{
  TcpNewReno::SetSSThresh (threshold);

  m_initSSTh = m_ssThresh;
}

void
TcpHybla::RecalcParam ()
{
  Time rtt = m_lastRtt.Get ();

  if (rtt.IsZero ())
    {
      return;
    }

  m_rho = std::max ((double)rtt.GetMilliSeconds () / m_rRtt.GetMilliSeconds (), 1.0);

  /*
  if (m_ssThresh == m_initSSTh)
    {
       m_ssThresh *= m_rho;
    }

  if (m_cWnd == m_initialCWnd * m_segmentSize)
    {
      m_cWnd *= m_rho;
    }
*/

  NS_LOG_DEBUG ("RHO: " << m_rho << " rtt= " << rtt.GetMilliSeconds ());
  NS_LOG_DEBUG ("cWnd: " << m_cWnd << " ssThresh: " << m_ssThresh);
}

void
TcpHybla::NewAck (const SequenceNumber32 &seq)
{
  NS_LOG_FUNCTION (this);

  double increment;
  bool is_slowstart = false;

  Time rtt = m_lastRtt.Get ();
  if (rtt.IsZero ())
    {
      rtt = m_rtt->GetCurrentEstimate ();
    }

  /*  Recalculate rho only if this srtt is the lowest */
  if (rtt < m_minRtt)
    {
      RecalcParam ();
      m_minRtt = rtt;
    }

  if (m_cWnd.Get () < m_ssThresh)
    {
      /*
       * slow start
       *      INC = 2^RHO - 1
       */
      is_slowstart = true;
      NS_ASSERT (m_rho > 0.0);
      increment = std::pow (2, m_rho) - 1;
      NS_LOG_DEBUG ("SS");
    }
  else
    {
      /*
       * congestion avoidance
       * INC = RHO^2 / W
       */
      NS_LOG_DEBUG ("CA");
      NS_ASSERT (m_cWnd.Get () != 0);
      increment = std::pow (m_rho, 2) / ((double) m_cWnd.Get () / m_segmentSize);
    }

  NS_ASSERT (increment >= 0.0);
  NS_LOG_DEBUG ("incr=" << increment);

  NS_LOG_DEBUG ("Old cWnd: " << m_cWnd);
  m_cWnd += m_segmentSize * increment;

  /* clamp down slowstart cwnd to ssthresh value. */
  if (is_slowstart)
    {
      m_cWnd = std::min (m_cWnd.Get (), m_ssThresh);
    }

  NS_LOG_DEBUG ("New=" << m_cWnd);

  TcpSocketBase::NewAck (seq);
}

Ptr<TcpSocketBase>
TcpHybla::Fork (void)
{
  return CopyObject<TcpHybla> (this);
}


} // namespace ns3
