#ifndef DCCPOPTION_H
#define DCCPOPTION_H
#include <stdint.h>
#include "ns3/object.h"
#include "ns3/buffer.h"
#include "ns3/object-factory.h"

namespace ns3 {

class DccpOption : public Object
{
public:

  DccpOption ();
  virtual ~DccpOption ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  enum Kind
  {
    // Remember to extend IsKindKnown() with new value, when adding values here
    PADDING = 0,      //!< padding option (type 0)
    UNKNOWN = 255     //!< not a standardized value; for unknown recv'd options
  };

  virtual void Print (std::ostream &os) const = 0;
  virtual void Serialize (Buffer::Iterator start) const = 0;
  virtual uint32_t Deserialize (Buffer::Iterator start) = 0;

  /**
   * \brief Get the option type
   * \return The scale value
   */
  virtual uint8_t GetKind (void) const = 0;

  virtual uint8_t GetSerializedSize (void) const = 0;
  static Ptr<DccpOption> CreateOption (uint8_t kind);
  static bool IsKindKnown (uint8_t kind);
};

/**
 * \brief Defines a general class to manage DCCP header options
 */
class DccpOptionUnknown : public DccpOption
{
  public:

    DccpOptionUnknown ();
    virtual ~DccpOptionUnknown ();

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual void Print (std::ostream &os) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);

    /**
     * \brief Get the option type
     * \return The scale value
     */
    virtual uint8_t GetKind (void) const;
    virtual uint8_t GetSerializedSize (void) const;

  private:
    uint8_t m_kind; //!< The unknown option type
    uint8_t m_size; //!< The unknown option size
    uint8_t m_content[]; //!< The option data
};

}

#endif // DCCPOPTION_H
