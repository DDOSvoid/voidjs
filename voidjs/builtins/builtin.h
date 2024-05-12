#ifndef VOIDJS_BUILTINS_BUILTIN_H
#define VOIDJS_BUILTINS_BUILTIN_H

#include "voidjs/ir/ast.h"
#include "voidjs/types/js_value.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {

namespace types {

class LexicalEnvironment;

}  // namespace types

namespace builtins {

class JSFunction;

}  // namespace builtins
  
class VM;

namespace builtins {

class Builtin {
 public:
  static void InitializeBuiltinObjects(VM* vm);
  static void InitializeBaseObjects(VM* vm);
  static void InitializeArrayObjects(VM* vm);
  static void InitializeStringObjects(VM* vm);
  static void InitializeBooleanObjects(VM* vm);
  static void InitializeNumberObjects(VM* vm);
  static void InitializeMathObjects(VM* vm);
  static void InitializeErrorObjects(VM* vm);

  static JSHandle<JSFunction> InstantiatingFunctionDeclaration(
    VM* vm, ast::AstNode* ast_node,
    JSHandle<types::LexicalEnvironment> scope, bool strict);

  static void SetPropretiesForBuiltinObjects(VM* vm);
  static void SetPropertiesForBaseObjects(VM* vm);
  static void SetPropertiesForArrayObjectss(VM* vm);
  static void SetProeprtiesForStringObjects(VM* vm);
  static void SetPropertiesForBooleanObjects(VM* vm);
  static void SetPropertiesForNumberObjects(VM* vm);
  static void SetPropertiesForMathObjects(VM* vm);
  static void SetDataProperty(VM* vm, JSHandle<types::Object> obj, JSHandle<types::String> prop_name, JSHandle<JSValue> prop_val,
                              bool writable, bool enumerable, bool configurable);
  static void SetFunctionProperty(VM* vm, JSHandle<types::Object> obj, JSHandle<types::String> prop_name, InternalFunctionType func,
                                 bool writable, bool enumerable, bool configurable);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_BUILTIN_H
