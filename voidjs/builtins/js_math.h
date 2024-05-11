#ifndef VOIDJS_BUILTINS_JS_MATH_H
#define VOIDJS_BUILTINS_JS_MATH_H

#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

// The Math object is a single object that has some named properties, some of which are functions.
// No Math Constructor and Math Prototype
class JSMath : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = Object::END_OFFSET + SIZE;

  // Function properties of the Math Object
  static JSValue Abs(RuntimeCallInfo* argv);
  static JSValue Acos(RuntimeCallInfo* argv);
  static JSValue Asin(RuntimeCallInfo* argv);
  static JSValue Atan(RuntimeCallInfo* argv);
  static JSValue Atan2(RuntimeCallInfo* argv);
  static JSValue Ceil(RuntimeCallInfo* argv);
  static JSValue Cos(RuntimeCallInfo* argv);
  static JSValue Exp(RuntimeCallInfo* argv);
  static JSValue Floor(RuntimeCallInfo* argv);
  static JSValue Log(RuntimeCallInfo* argv);
  static JSValue Max(RuntimeCallInfo* argv);
  static JSValue Min(RuntimeCallInfo* argv);
  static JSValue Pow(RuntimeCallInfo* argv);
  static JSValue Random(RuntimeCallInfo* argv);
  static JSValue Round(RuntimeCallInfo* argv);
  static JSValue Sin(RuntimeCallInfo* argv);
  static JSValue Sqrt(RuntimeCallInfo* argv);
  static JSValue Tan(RuntimeCallInfo* argv);
};

}  // namespace builtins
}  // namespace voidjs



#endif  // VOIDJS_BUILTINS_JS_MATH_H
