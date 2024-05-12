#ifndef VOIDJS_BUILTINS_JS_NUMBER_H
#define VOIDJS_BUILTINS_JS_NUMBER_H

#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

class JSNumber : public types::Object {
 public:
  static constexpr std::size_t PRIMITIVE_VALUE_OFFSET = types::Object::END_OFFSET;
  JSValue GetPrimitiveValue() const { return *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET); }
  void SetPrimitiveValue(JSValue value) { *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET) = value; } 
  void SetPrimitiveValue(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET) = handle.GetJSValue(); } 
  
  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;

  // Internal method [[Construct]] and [[Call]] for Array Constructor
  static JSValue NumberConstructorCall(RuntimeCallInfo*);
  static JSValue NumberConstructorConstruct(RuntimeCallInfo*);

  // Function propreties of the Number Prototype
  
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_JS_NUMBER_H
