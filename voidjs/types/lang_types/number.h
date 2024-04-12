#ifndef VOIDJS_TYPES_LANG_TYPES_NUMBER_H
#define VOIDJS_TYPES_LANG_TYPES_NUMBER_H

#include "voidjs/types/js_value.h"

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
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_NUMBER_H
