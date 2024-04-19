#ifndef VOIDJS_TYPES_LANG_TYPES_STRING_H
#define VOIDJS_TYPES_LANG_TYPES_STRING_H

#include <algorithm>
#include <cstdint>
#include <string>

#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/heap_object.h"

namespace voidjs {
namespace types {

class String : public HeapObject {
 public:
  // std::size_t length_;
  static constexpr std::size_t LENGTH_OFFSET = HeapObject::SIZE;
  std::size_t GetLength() const { return *utils::BitGet<std::size_t*>(this, LENGTH_OFFSET); }
  void SetLength(std::size_t length) { *utils::BitGet<std::size_t*>(this, LENGTH_OFFSET) = length; }

  // char16_t[] data_;
  static constexpr std::size_t DATA_OFFSET = LENGTH_OFFSET + sizeof(std::size_t);
  char16_t* GetData() const { return utils::BitGet<char16_t*>(this, DATA_OFFSET); }
  char16_t GetByIndex(std::size_t idx) const { return *(GetData() + idx); } 
  void SetByIndex(std::size_t idx, char16_t ch) { *(GetData() + idx) = ch; }

  // used for print 
  std::u16string_view GetString() const {
    auto len = GetLength();
    auto data = GetData();
    return std::u16string_view(data, len);
  }
  
  static String* New(const std::u16string& source) {
    auto len = source.size();
    auto str = reinterpret_cast<String*>(
      HeapObject::New(sizeof(std::size_t) + len * sizeof(char16_t),
                  JSType::STRING));
    str->SetLength(len);
    std::copy(source.begin(), source.end(), str->GetData());
    return str;
  }

};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_STRING_H
