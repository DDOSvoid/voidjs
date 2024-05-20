#ifndef VOIDJS_BUILTINS_JS_ARRAY_H
#define VOIDJS_BUILTINS_JS_ARRAY_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

class JSArray : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;

  static bool DefineOwnProperty(
    VM* vm, JSHandle<types::Object> O, JSHandle<types::String> P, const types::PropertyDescriptor& Desc, bool Throw);

  // Internal method [[Construct]] and [[Call]] for Array Constructor
  static JSValue ArrayConstructorCall(RuntimeCallInfo* argv);
  static JSValue ArrayConstructorConstruct(RuntimeCallInfo* argv);

  // Function properties of the Array Constructor
  static JSValue IsArray(RuntimeCallInfo* argv);

  // Function propreties of the Array Prototype
  static JSValue ToString(RuntimeCallInfo* argv);
  static JSValue ToLocaleString(RuntimeCallInfo* argv);
  static JSValue Concat(RuntimeCallInfo* argv);
  static JSValue Join(RuntimeCallInfo* argv);
  static JSValue Pop(RuntimeCallInfo* argv);
  static JSValue Push(RuntimeCallInfo* argv);
  static JSValue Reverse(RuntimeCallInfo* argv);
  static JSValue Shift(RuntimeCallInfo* argv);
  static JSValue Slice(RuntimeCallInfo* argv);
  static JSValue Sort(RuntimeCallInfo* argv);
  static JSValue ForEach(RuntimeCallInfo* argv);
  static JSValue Map(RuntimeCallInfo* argv);
  static JSValue Filter(RuntimeCallInfo* argv);
};

}  // namespace builtins
}  // namespace voidjs


#endif  // VOIDJS_BUILTINS_JS_ARRAY_H
