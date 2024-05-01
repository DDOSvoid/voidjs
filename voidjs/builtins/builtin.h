#ifndef VOIDJS_BUILTINS_BUILTIN_H
#define VOIDJS_BUILTINS_BUILTIN_H

#include "voidjs/types/js_value.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {

class VM;

namespace builtins {

class Builtin {
 public:
  static void Initialize(VM* vm);
  static void InitializeBuiltinObjects(VM* vm);
  static void InitializeErrorObjects(VM* vm);
  
 private:
  static void SetPropretiesForBuiltinObjects(VM* vm);
  static void SetDataProperty(VM* vm, types::Object* obj, types::String* prop_name, JSValue prop_val,
                              bool writable, bool enumerable, bool configurable);
  static void SetFunctionProperty(VM* vm, types::Object* obj, types::String* prop_name, InternalFunctionType func);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_BUILTIN_H
