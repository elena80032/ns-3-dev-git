/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: Rajib Bhattacharjea<raj.b@gatech.edu>
//


// Ported from:
// Georgia Tech Network Simulator - Round Trip Time Estimation Class
// George F. Riley.  Georgia Tech, Spring 2002

// Implements several variations of round trip time estimators

#include <iostream>

#include "rtt-estimator.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RttEstimator");

NS_OBJECT_ENSURE_REGISTERED (RttEstimator);

TypeId 
RttEstimator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttEstimator")
    .SetParent<Object> ()
    .AddAttribute ("InitialEstimation", 
                   "Initial RTT estimation",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&RttEstimator::m_initialEstimatedRtt),
                   MakeTimeChecker ())
  ;
  return tid;
}

Time
RttEstimator::GetEstimate (void) const
{
  return m_estimatedRtt;
}

Time 
RttEstimator::GetEstimateVariation (void) const
{
  return m_estimatedRttVariation;
}


// Base class methods

RttEstimator::RttEstimator ()
  : m_nSamples (0)
{ 
  NS_LOG_FUNCTION (this);
  
  // We need attributes initialized here, not later, so use the 
  // ConstructSelf() technique documented in the manual
  ObjectBase::ConstructSelf (AttributeConstructionList ());
  m_estimatedRtt = m_initialEstimatedRtt;
  NS_LOG_DEBUG ("Initialize m_estimatedRtt to " << m_estimatedRtt.GetSeconds () << " sec.");
}

RttEstimator::RttEstimator (const RttEstimator& c)
  : Object (c),
    m_initialEstimatedRtt (c.m_initialEstimatedRtt),
    m_estimatedRtt (c.m_estimatedRtt),
    m_estimatedRttVariation (c.m_estimatedRttVariation),
    m_nSamples (c.m_nSamples)
{
  NS_LOG_FUNCTION (this);
}

RttEstimator::~RttEstimator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
RttEstimator::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void RttEstimator::Reset ()
{ 
  NS_LOG_FUNCTION (this);
  // Reset to initial state
  m_estimatedRtt = m_initialEstimatedRtt;
  m_estimatedRttVariation = Time (0);
  m_nSamples = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Mean-Deviation Estimator

NS_OBJECT_ENSURE_REGISTERED (RttMeanDeviation);

TypeId 
RttMeanDeviation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttMeanDeviation")
    .SetParent<RttEstimator> ()
    .AddConstructor<RttMeanDeviation> ()
    .AddAttribute ("Alpha",
                   "Gain used in estimating the RTT, must be 0 < alpha < 1",
                   DoubleValue (0.125),
                   MakeDoubleAccessor (&RttMeanDeviation::m_alpha),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Beta",
                   "Gain used in estimating the RTT variance, must be 0 < beta < 1",
                   DoubleValue (0.25),
                   MakeDoubleAccessor (&RttMeanDeviation::m_beta),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

RttMeanDeviation::RttMeanDeviation()
{
  NS_LOG_FUNCTION (this);
}

RttMeanDeviation::RttMeanDeviation (const RttMeanDeviation& c)
  : RttEstimator (c), m_alpha (c.m_alpha), m_beta (c.m_beta)
{
  NS_LOG_FUNCTION (this);
}

TypeId
RttMeanDeviation::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void RttMeanDeviation::Measurement (Time m)
{
  NS_LOG_FUNCTION (this << m);
  if (m_nSamples)
    { // Not first sample
      Time err (m - m_estimatedRtt);
      double gErr = err.ToDouble (Time::S) * m_alpha;
      m_estimatedRtt += Time::FromDouble (gErr, Time::S);

      Time difference = Abs (err) - m_estimatedRttVariation;
      NS_LOG_DEBUG ("m_estimatedRttVariation += " << Time::FromDouble (difference.ToDouble (Time::S) * m_beta, Time::S));
      m_estimatedRttVariation += Time::FromDouble (difference.ToDouble (Time::S) * m_beta, Time::S);
    }
  else
    { // First sample
      m_estimatedRtt = m;               // Set estimate to current
      m_estimatedRttVariation = m / 2;  // And variation to current / 2
      NS_LOG_DEBUG ("(first sample) m_estimatedRttVariation += " << m);
    }
  m_nSamples++;
}

//Time RttMeanDeviation::RetransmitTimeout ()
//{
//  NS_LOG_FUNCTION (this);
//  NS_LOG_DEBUG ("RetransmitTimeout:  var " << m_variance.GetSeconds () << " est " << m_currentEstimatedRtt.GetSeconds () << " multiplier " << m_multiplier);
//  // RTO = srtt + 4* rttvar
//  int64_t temp = m_currentEstimatedRtt.ToInteger (Time::MS) + 4 * m_variance.ToInteger (Time::MS);
//  if (temp < m_minRto.ToInteger (Time::MS))
//    {
//      temp = m_minRto.ToInteger (Time::MS);
//    }
//  temp = temp * m_multiplier; // Apply backoff
//  Time retval = Time::FromInteger (temp, Time::MS);
//  NS_LOG_DEBUG ("RetransmitTimeout:  return " << retval.GetSeconds ());
//  return (retval);
//}

Ptr<RttEstimator> RttMeanDeviation::Copy () const
{
  NS_LOG_FUNCTION (this);
  return CopyObject<RttMeanDeviation> (this);
}

void RttMeanDeviation::Reset ()
{ 
  NS_LOG_FUNCTION (this);
  RttEstimator::Reset ();
}

} //namespace ns3
