#ifndef VOIDJS_TYPES_LANG_TYPES_STRING_H
#define VOIDJS_TYPES_LANG_TYPES_STRING_H

#include <algorithm>
#include <cstdint>
#include <string>

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class String : public JSValue {
 public:
  explicit String(const std::u16string& str) {
    auto ptr = new std::uint8_t[sizeof(std::size_t) + sizeof(char16_t) * str.size()];
    
    std::copy(str.begin(), str.end(), reinterpret_cast<char16_t*>(ptr + sizeof(std::size_t)));
    
    *reinterpret_cast<std::size_t*>(ptr) = str.size();
      
    value_ = reinterpret_cast<JSValueType>(ptr);
  }

  std::u16string GetValue() const {
    auto ptr = reinterpret_cast<std::uint8_t*>(value_);
    auto sz = *reinterpret_cast<std::size_t*>(ptr);
    auto str = reinterpret_cast<char16_t*>(ptr + sizeof(std::size_t));
    return std::u16string(str, sz);
  }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_STRING_H
