#ifndef VOIDJS_TYPES_LANG_TYPES_OBJECT_H
#define VOIDJS_TYPES_LANG_TYPES_OBJECT_H

#include <cstddef>
#include <cstdint>
#include <optional>

#include "voidjs/utils/helper.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/spec_types/property_descriptor.h"

namespace voidjs {
namespace types {

class String;

// properties
// prototype
class Object : public HeapObject {
 public:
  static constexpr std::size_t PROPERTIES_OFFSET = HeapObject::SIZE;
  JSValue GetProperties() const { return *utils::BitGet<JSValue*>(this, PROPERTIES_OFFSET); }
  void SetProperties(JSValue props) { *utils::BitGet<JSValue*>(this, PROPERTIES_OFFSET) = props; }

  static constexpr std::size_t PROTOTYPE_OFFSET = PROPERTIES_OFFSET + sizeof(JSValue);
  JSValue GetProtoType() const { return *utils::BitGet<JSValue*>(this, PROTOTYPE_OFFSET); }
  void SetProtoType(JSValue proto) { *utils::BitGet<JSValue*>(this, PROTOTYPE_OFFSET) = proto; }

  static constexpr std::size_t SIZE = sizeof(JSValue) + sizeof(JSValue);
  static constexpr std::size_t OFFSET = PROTOTYPE_OFFSET + sizeof(JSValue);

  // Internal function Properties common to all Objects
  // Defined in ECMAScript 5.1 Chapter 8.12
  PropertyDescriptor GetOwnProperty(JSValue P) const;
  PropertyDescriptor GetProperty(JSValue P) const;
  JSValue Get(JSValue P) const;
  bool CanPut(JSValue P) const;
  void Put(JSValue P, JSValue V, bool Throw);
  bool HasProperty(JSValue P) const;
  bool Delete(JSValue P, bool Throw);
  JSValue DefaultValue(PreferredType hint) const;
  bool DefineOwnProperty(JSValue P, const PropertyDescriptor& Desc, bool Throw); 
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANG_TYPES_OBJECT_H
