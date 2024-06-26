#ifndef VOIDJS_BUILTINS_JS_FUNCTION_H
#define VOIDJS_BUILTINS_JS_FUNCTION_H

#include "voidjs/ir/ast.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/utils/helper.h"

namespace voidjs {

class RuntimeCallInfo;

namespace builtins {

class JSFunction : public types::Object {
 public:
  static constexpr std::size_t CODE_OFFSET = Object::END_OFFSET;
  ast::AstNode* GetCode() const { return *utils::BitGet<ast::AstNode**>(this, CODE_OFFSET); }
  void SetCode(ast::AstNode* ast_node) { *utils::BitGet<ast::AstNode**>(this, CODE_OFFSET) = ast_node; }

  // LexicalEnvironment*
  static constexpr std::size_t SCOPE_OFFSET = CODE_OFFSET + sizeof(std::uintptr_t);
  JSValue GetScope() const { return *utils::BitGet<JSValue*>(this, SCOPE_OFFSET); }
  void SetScope(JSValue value) { *utils::BitGet<JSValue*>(this, SCOPE_OFFSET) = value; }
  void SetScope(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, SCOPE_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t SIZE = sizeof(std::uintptr_t) + sizeof(std::uintptr_t);
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
  static_assert(SIZE == 16);

  // [[HasInstance]](V)
  static bool HasInstance(VM* vm, JSHandle<JSFunction> F, JSHandle<JSValue> V);
  static JSHandle<JSValue> Get(VM* vm, JSHandle<JSFunction> O, JSHandle<types::String> P);
  
  // The Function Constructor
  static JSValue FunctionConstructorCall(RuntimeCallInfo* argv);
  static JSValue FunctionConstructorConstruct(RuntimeCallInfo* argv);

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
