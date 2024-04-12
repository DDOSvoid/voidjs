#ifndef VOIDJS_TYPES_LANG_TYPES_STRING_H
#define VOIDJS_TYPES_LANG_TYPES_STRING_H

#include <algorithm>
#include <string> 

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class String : public JSValue {
 public:
  explicit String(std::u16string str) {
    auto ptr = new char16_t[str.size() + 1];
    std::copy(str.begin(), str.end(), ptr);
    value_ = reinterpret_cast<JSValueType>(ptr);
  }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_STRING_H
