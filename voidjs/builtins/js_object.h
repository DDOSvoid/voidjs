#ifndef VOIDJS_BUILTINS_JS_OBJECT_H
#define VOIDJS_BUILTINS_JS_OBJECT_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace builtins {

class JSObject : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
  static_assert(SIZE == 0);

  // Internal method [[Construct]] and [[Call]] for Object Constructor
  static JSValue ObjectConstructorConstruct(RuntimeCallInfo*);
  static JSValue ObjectConstructorCall(RuntimeCallInfo*);

  // Function properties of the Object Constructor
  // Defined in ECMAScript 5.1 Chapter 15.2.3
  static JSValue GetPrototypeOf(RuntimeCallInfo* argv);
  static JSValue GetOwnPropertyDescriptor(RuntimeCallInfo* argv);
  static JSValue GetOwnPropertyNames(RuntimeCallInfo* argv);
  static JSValue Create(RuntimeCallInfo* argv);
  static JSValue DefineProperty(RuntimeCallInfo* argv);
  static JSValue DefineProperties(RuntimeCallInfo* argv);
  static JSValue Seal(RuntimeCallInfo* argv);
  static JSValue Freeze(RuntimeCallInfo* argv);
  static JSValue PreventExtensions(RuntimeCallInfo* argv);
  static JSValue IsSealed(RuntimeCallInfo* argv);
  static JSValue IsFrozen(RuntimeCallInfo* argv);
  static JSValue IsExtensible(RuntimeCallInfo* argv);
  static JSValue Keys(RuntimeCallInfo* argv);

  // Function properties of the Object Prototype
  // Defined in ECMAScript 5.1 Chapter 15.2.4
  static JSValue ToString(RuntimeCallInfo* argv);
  static JSValue ToLocaleString(RuntimeCallInfo* argv);
  static JSValue ValueOf(RuntimeCallInfo* argv);
  static JSValue HasOwnProperty(RuntimeCallInfo* argv);
  static JSValue IsPrototypeOf(RuntimeCallInfo* argv);
  static JSValue PropertyIsEnumerable(RuntimeCallInfo* argv);
};

}  // namespace builtin
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_JS_OBJECT_H
