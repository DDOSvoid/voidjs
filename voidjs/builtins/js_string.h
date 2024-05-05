#ifndef VOIDJS_BUILTINS_JS_STRING_H
#define VOIDJS_BUILTINS_JS_STRING_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

class JSString : public types::Object {
 public:
  static constexpr std::size_t PRIMITIVE_VALUE_OFFSET = types::Object::END_OFFSET;
  JSValue GetPrimitiveValue() const { return *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET); }
  void SetPrimitiveValue(JSValue val) { *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET) = val; } 
  
  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;

  // Internal method [[Construct]] and [[Call]] for String Constructor
  static JSValue Call(RuntimeCallInfo* argv);
  static JSValue Construct(RuntimeCallInfo* argv);

  // Function properties of String Constructor
  static JSValue FromCharCode(RuntimeCallInfo* argv);

  // Function properties of String Prototype
  static JSValue ToString(RuntimeCallInfo* argv);
  static JSValue ValueOf(RuntimeCallInfo* argv);
  static JSValue CharAt(RuntimeCallInfo* argv);
  static JSValue CharCodeAt(RuntimeCallInfo* argv);
  static JSValue Concat(RuntimeCallInfo* argv);
  static JSValue IndexOf(RuntimeCallInfo* argv);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_JS_STRING_H
