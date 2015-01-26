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

// Georgia Tech Network Simulator - Round Trip Time Estimation Class
// George F. Riley.  Georgia Tech, Spring 2002


#ifndef RTT_ESTIMATOR_H
#define RTT_ESTIMATOR_H

#include <deque>
#include "ns3/sequence-number.h"
#include "ns3/nstime.h"
#include "ns3/object.h"

namespace ns3 {

class Packet;
class TcpHeader;

/**
 * \ingroup tcp
 *
 * \brief Base class for all RTT Estimators
 */
class RttEstimator : public Object {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RttEstimator();
  /**
   * \brief Copy constructor
   * \param r the object to copy
   */
  RttEstimator (const RttEstimator& r);

  virtual ~RttEstimator();

  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * \brief Add a new measurement to the estimator. Pure virtual function.
   * \param t the new RTT measure.
   */
  virtual void  Measurement (Time t) = 0;

  /**
   * \brief Copy object
   * \returns a copy of itself
   */
  virtual Ptr<RttEstimator> Copy () const = 0;

  /**
   * \brief Resets the estimation to its initial state.
   */
  virtual void Reset ();

  /**
   * \brief gets the RTT estimate.
   * \return The RTT estimate.
   */
  Time GetEstimate (void) const;

  /**
   * \brief gets the RTT estimate variation.
   * \return The RTT estimate variation.
   */
  Time GetEstimateVariation (void) const;

private:
  Time m_initialEstimatedRtt; //!< Initial RTT estimation

protected:
  Time         m_estimatedRtt;            //!< Current estimate
  Time         m_estimatedRttVariation;   //!< Current estimate variation
  uint32_t     m_nSamples;                //!< Number of samples
};

/**
 * \ingroup tcp
 *
 * \brief The "Mean--Deviation" RTT estimator, as discussed by Van Jacobson
 *
 * This class implements the "Mean--Deviation" RTT estimator, as discussed
 * by Van Jacobson and Michael J. Karels, in
 * "Congestion Avoidance and Control", SIGCOMM 88, Appendix A
 *
 */
class RttMeanDeviation : public RttEstimator {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RttMeanDeviation ();

  /**
   * \brief Copy constructor
   * \param r the object to copy
   */
  RttMeanDeviation (const RttMeanDeviation& r);

  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * \brief Add a new measurement to the estimator.
   * \param measure the new RTT measure.
   */
  void Measurement (Time measure);

  Ptr<RttEstimator> Copy () const;

  /**
   * \brief Resets the estimator.
   */
  void Reset ();

private:
  double       m_alpha;       //!< Filter gain for average
  double       m_beta;        //!< Filter gain for variance
};
} // namespace ns3

#endif /* RTT_ESTIMATOR_H */
