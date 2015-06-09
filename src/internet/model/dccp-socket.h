#ifndef DCCP_SOCKET_H
#define DCCP_SOCKET_H

#include "ns3/socket.h"
#include "ns3/traced-callback.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/object.h"

namespace ns3 {

class Node;
class Packet;


class DccpSocket : public Socket
{
public:

  static TypeId GetTypeId (void);

  DccpSocket (void);
  virtual ~DccpSocket (void);

private:
  // Indirect the attribute setting and getting through private virtual methods

  virtual void SetRcvBufSize (uint32_t size) = 0;
  virtual uint32_t GetRcvBufSize (void) const = 0;
  virtual void SetMtuDiscover (bool discover) = 0;
  virtual bool GetMtuDiscover (void) const = 0;

};

} // namespace ns3

#endif /* DCCP_SOCKET_H */


