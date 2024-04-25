#ifndef VOIDJS_TYPES_LANG_TYPES_STRING_H
#define VOIDJS_TYPES_LANG_TYPES_STRING_H

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/heap_object.h"

namespace voidjs {
namespace types {

class String : public HeapObject {
 public:
  // std::size_t length_;
  static constexpr std::size_t LENGTH_OFFSET = HeapObject::OFFSET;
  std::size_t GetLength() const { return *utils::BitGet<std::size_t*>(this, LENGTH_OFFSET); }
  void SetLength(std::size_t length) { *utils::BitGet<std::size_t*>(this, LENGTH_OFFSET) = length; }

  // char16_t[] data_;
  static constexpr std::size_t DATA_OFFSET = LENGTH_OFFSET + sizeof(std::size_t);
  char16_t* GetData() const { return utils::BitGet<char16_t*>(this, DATA_OFFSET); }
  char16_t GetByIndex(std::size_t idx) const { return *(GetData() + idx); } 
  void SetByIndex(std::size_t idx, char16_t ch) { *(GetData() + idx) = ch; }

  bool Equal(std::u16string_view str) const { return GetString() == str; }
  bool Equal(String* str) const { return Equal(str); }

  // used for print 
  std::u16string_view GetString() const {
    auto len = GetLength();
    auto data = GetData();
    return std::u16string_view(data, len);
  }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_STRING_H
