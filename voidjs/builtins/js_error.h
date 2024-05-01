#ifndef VOIDJS_BUILTINS_JS_ERROR_H
#define VOIDJS_BUILTINS_JS_ERROR_H

#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace builtins {

class JSError : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
  static_assert(SIZE == 0);

  // The Error object internal method [[Construct]] and [[Call]]
  static JSError* Construct(VM* vm, JSValue value);
  static JSValue Call(VM* vm, JSValue value);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VODIJS_BUILTINS_JS_ERROR_H
