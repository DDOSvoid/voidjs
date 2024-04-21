#ifndef VOIDJS_TYPES_INTERNAL_TYPES_ARRAY_H
#define VOIDJS_TYPES_INTERNAL_TYPES_ARRAY_H

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class Array : public HeapObject {
 public:
  // std::size_t length_;
  static constexpr std::size_t LENGTH_OFFSET = HeapObject::OFFSET;
  std::size_t GetLength() const { return *utils::BitGet<std::size_t*>(this, LENGTH_OFFSET); }
  void SetLength(std::size_t length) { *utils::BitGet<std::size_t*>(this, LENGTH_OFFSET) = length; }

  // JSValue[] data_;
  static constexpr std::size_t DATA_OFFSET = LENGTH_OFFSET + sizeof(std::size_t);
  JSValue* GetData() const { return utils::BitGet<JSValue*>(this, DATA_OFFSET); }
  JSValue Get(std::size_t idx) const { return *(GetData() + idx); } 
  void Set(std::size_t idx, JSValue val) { *(GetData() + idx) = val; }

  static Array* Append(Array* first, Array* second);
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_ARRAY_H
