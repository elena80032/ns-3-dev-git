#ifndef DCCPOPTIONPADDING_H
#define DCCPOPTIONPADDING_H

#include <stdint.h>
#include "ns3/object.h"
#include "ns3/buffer.h"
#include "ns3/object-factory.h"
#include "dccp-option.h"

namespace ns3{

  /**
   * \brief Defines the DCCP option of type 0 (padding option) as in \RFC{4340}
   */

class DccpOptionPadding : public DccpOption
{
  public:

    DccpOptionPadding ();
    virtual ~DccpOptionPadding ();
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;

    /**
     * \brief Get the option type
     * \return The scale value
     */
    virtual uint8_t GetKind (void) const;

    virtual void Print (std::ostream &os) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual uint8_t GetSerializedSize (void) const;
};

}
#endif // DCCPOPTIONPADDING_H
