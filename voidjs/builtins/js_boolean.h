#ifndef VOIDJS_BUILTINS_BOOLEAN_H
#define VOIDJS_BUILTINS_BOOLEAN_H

#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace builtins {

class JSBoolean : public types::Object {
 public:
  static constexpr std::size_t PRIMITIVE_VALUE_OFFSET = types::Object::END_OFFSET;
  JSValue GetPrimitiveValue() const { return *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET); }
  void SetPrimitiveValue(JSValue val) { *utils::BitGet<JSValue*>(this, PRIMITIVE_VALUE_OFFSET) = val; } 
  
  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;

  // Internal method [[Construct]] and [[Call]] for Boolean Constructor
  static JSValue BooleanConstructorCall(RuntimeCallInfo* argv);
  static JSValue BooleanConstructorConstruct(RuntimeCallInfo* argv);

  // Function properties of the Boolean Prototype
  static JSValue ToString(RuntimeCallInfo* argv);
  static JSValue ValueOf(RuntimeCallInfo* argv);
}; 

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BULTINS_BOOELAN_H
