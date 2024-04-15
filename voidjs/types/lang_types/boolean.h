#ifndef VOIDJS_TYPES_LANG_TYPES_BOOLEAN_H
#define VOIDJS_TYPES_LANG_TYPES_BOOLEAN_H

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class Boolean: public JSValue {
 public:
  Boolean()
    : JSValue()
  {}

  explicit Boolean(bool boolean)
    : JSValue(boolean)
  {}

  bool GetValue() const { return IsTrue(); }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_BOOLEAN_NULL_H
