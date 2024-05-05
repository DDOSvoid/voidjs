#ifndef VOIDJS_BUILTINS_BUILTIN_H
#define VOIDJS_BUILTINS_BUILTIN_H

#include "voidjs/ir/ast.h"
#include "voidjs/types/js_value.h"
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
  static void Initialize(VM* vm);
  static void InitializeBuiltinObjects(VM* vm);
  static void InitializeArrayObjects(VM* vm);
  static void InitializeStringObjects(VM* vm);
  static void InitializeBooleanObjects(VM* vm);
  static void InitializeNumberObjects(VM* vm);
  static void InitializeErrorObjects(VM* vm);

  static builtins::JSFunction* InstantiatingFunctionDeclaration(
    VM* vm, ast::AstNode* ast_node,
    types::LexicalEnvironment* scope, bool strict);
  
  static void SetPropretiesForBuiltinObjects(VM* vm);
  static void SetDataProperty(VM* vm, types::Object* obj, types::String* prop_name, JSValue prop_val,
                              bool writable, bool enumerable, bool configurable);
  static void SetFunctionProperty(VM* vm, types::Object* obj, types::String* prop_name, InternalFunctionType func,
                                 bool writable, bool enumerable, bool configurable);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_BUILTIN_H
