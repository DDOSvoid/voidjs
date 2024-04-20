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

class Object : public HeapObject {
 public:
  static constexpr std::size_t PROPERTIES_OFFSET = HeapObject::SIZE;
  JSValue GetProperties() const { return *utils::BitGet<JSValue*>(this, PROPERTIES_OFFSET); }
  void SetProperties(JSValue props) { *utils::BitGet<JSValue*>(this, PROPERTIES_OFFSET) = props; }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t OFFSET = PROPERTIES_OFFSET + sizeof(JSValue);
  
  std::optional<PropertyDescriptor> GetOwnProperty(JSValue P) const;
  JSValue Get() const;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANG_TYPES_OBJECT_H
