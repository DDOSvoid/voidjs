#ifndef VOIDJS_TYPES_INTERNAL_TYPES_BINDING_H
#define VOIDJS_TYPES_INTERNAL_TYPES_BINDING_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace types {

class Binding : public HeapObject {
 public:
  static constexpr std::size_t VALUE_OFFSET = HeapObject::END_OFFSET;
  JSValue GetValue() const { return *utils::BitGet<JSValue*>(this, VALUE_OFFSET); }
  void SetValue(JSValue value) const { *utils::BitGet<JSValue*>(this, VALUE_OFFSET) = value; }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_BINDING_H
