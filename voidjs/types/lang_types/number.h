#ifndef VOIDJS_TYPES_LANG_TYPES_NUMBER_H
#define VOIDJS_TYPES_LANG_TYPES_NUMBER_H

#include "voidjs/types/js_value.h"
#include <type_traits>

namespace voidjs {
namespace types {

class Number : public JSValue {
 public:
  Number()
    : JSValue()
  {}

  explicit Number(std::int32_t number)
    : JSValue(number)
  {}

  explicit Number(double number)
    : JSValue(number)
  {}

  template <typename T>
  T GetValue() const {
    static_assert(std::is_same_v<std::int32_t, T> ||
                  std::is_same_v<double, T>);
    if constexpr (std::is_same_v<std::int32_t, T>) {
      return GetInt();
    } else {
      return GetDouble();
    }
  } 
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_NUMBER_H
