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
  /**
  * Get the type ID.
  * \brief Get the type ID.
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);

  DccpSocket (void);
  virtual ~DccpSocket (void);

private:
  // Indirect the attribute setting and getting through private virtual methods
  /**
  * \brief Set the receiving buffer size
  * \param size the buffer size
  */
  virtual void SetRcvBufSize (uint32_t size) = 0;
  /**
  * \brief Get the receiving buffer size
  * \returns the buffer size
  */
  virtual uint32_t GetRcvBufSize (void) const = 0;
  /**
  * \brief Set the MTU discover capability
  *
  * \param discover the MTU discover capability
  */
  virtual void SetMtuDiscover (bool discover) = 0;
  /**
  * \brief Get the MTU discover capability
  *
  * \returns the MTU discover capability
  */
  virtual bool GetMtuDiscover (void) const = 0;

};

} // namespace ns3

#endif /* DCCP_SOCKET_H */



