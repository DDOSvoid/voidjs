#ifndef VOIDJS_BUILTINS_JS_ERROR_H
#define VOIDJS_BUILTINS_JS_ERROR_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

class JSError : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
  static_assert(SIZE == 0);

  // The Error object internal method [[Construct]] and [[Call]]
  static JSValue ErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue ErrorConstructorCall(RuntimeCallInfo* argv);

  // The NativeError Object internal method [[Construct]] and [[Call]]
  static JSValue EvalErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue RangeErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue ReferenceErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue SyntaxErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue TypeErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue URIErrorConstructorConstruct(RuntimeCallInfo* argv);
  static JSValue EvalErrorConstructorCall(RuntimeCallInfo* argv);
  static JSValue RangeErrorConstructorCall(RuntimeCallInfo* argv);
  static JSValue ReferenceErrorConstructorCall(RuntimeCallInfo* argv);
  static JSValue SyntaxErrorConstructorCall(RuntimeCallInfo* argv);
  static JSValue TypeErrorConstructorCall(RuntimeCallInfo* argv);
  static JSValue URIErrorConstructorCall(RuntimeCallInfo* argv);

  // Function properties of the Error Prototype
  static JSValue ToString(RuntimeCallInfo* argv);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VODIJS_BUILTINS_JS_ERROR_H
