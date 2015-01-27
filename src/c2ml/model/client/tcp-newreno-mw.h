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
#ifndef TCPNEWRENOMW_H
#define TCPNEWRENOMW_H

#include "ns3/tcp-newreno.h"
#include "tcp-mw.h"
#include "ns3/timer.h"

namespace ns3 {

class CCL45Protocol;

class TcpNewRenoMw : public TcpNewReno,
                     public TcpMw
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpNewRenoMw ();

  void SetBandwidth (uint32_t b);

  Ptr<CCL45Protocol> GetMwProtocol ();

protected:
  virtual int Connect (const Address &address);
  virtual void CloseAndNotify (void);
  virtual void NewAck (SequenceNumber32 const& seq);
  virtual bool SendPendingData (bool withAck = false);

  uint32_t m_availableBw;
  Time m_minRtt;

};


} // namespace ns3

#endif // TCPNEWRENOMW_H
