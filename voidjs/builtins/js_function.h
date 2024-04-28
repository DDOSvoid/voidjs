#ifndef VOIDJS_BUILTINS_JS_FUNCTION_H
#define VOIDJS_BUILTINS_JS_FUNCTION_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"

namespace voidjs {

class RuntimeCallInfo;

namespace builtins {

class JSFunction : public types::Object {
 public:

  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t OFFSET = types::Object::OFFSET + SIZE;
  
  // The Function Constructor
  static JSValue FunctionConsturctor(RuntimeCallInfo* argv);

  // Function properties of the Function Constructor

  // Function properties of the Function Prototype
  static JSValue ToString(RuntimeCallInfo* argv);
  static JSValue Apply(RuntimeCallInfo* argv);
  static JSValue Call(RuntimeCallInfo* argv);
  static JSValue Bind(RuntimeCallInfo* argv);
  
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_JS_FUNCTION_H
