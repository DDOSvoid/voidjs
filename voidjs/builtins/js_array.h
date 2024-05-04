#ifndef VOIDJS_BUILTINS_JS_ARRAY_H
#define VOIDJS_BUILTINS_JS_ARRAY_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace builtins {

class JSArray : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;

  static bool DefineOwnProperty(VM* vm, types::Object* O, types::String* P, const types::PropertyDescriptor& Desc, bool Throw);

  // Internal method [[Construct]] and [[Call]] for Array Constructor
  static JSValue Call(RuntimeCallInfo* argv);
  static JSValue Construct(RuntimeCallInfo* argv);

  // Function properties of the Array Constructor
  static JSValue IsArray(RuntimeCallInfo* argv);

  // Function propreties of the Array Prototype
  static JSValue ToString(RuntimeCallInfo* argv);
  static JSValue ToLocaleString(RuntimeCallInfo* argv);
  static JSValue Concat(RuntimeCallInfo* argv);
  
};

}  // namespace builtins
}  // namespace voidjs


#endif  // VOIDJS_BUILTINS_JS_ARRAY_H
