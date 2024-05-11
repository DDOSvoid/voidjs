#include "voidjs/builtins/js_function.h"

#include "voidjs/ir/ast.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// Function (p1, p2, ..., pn, body)
// Defined in ECMAScript 5.1 Chapter 15.3.1.1
JSValue JSFunction::Call(RuntimeCallInfo* argv) {
  return Construct(argv);
}

// new Function (p1, p2, ..., pn, body)
// Defined in ECMAScript 5.1 Chapter 15.3.2.1
JSValue JSFunction::Construct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let argCount be the total number of arguments passed to this function invocation.
  std::size_t arg_count = argv->GetArgsNum();
  
  // 2. Let P be the empty String.
  std::u16string P = u"";
  JSHandle<JSValue> body;
  
  // 3. If argCount = 0, let body be the empty String.
  if (arg_count == 0) {
  }
  // 4. Else if argCount = 1, let body be that argument.
  else if (arg_count == 1) {
    body = argv->GetArg(0);
  }
  // 5. Else, argCount > 1
  else {
    // a. Let firstArg be the first argument.
    JSHandle<JSValue> first_arg = argv->GetArg(0);;
    
    // b. Let P be ToString(firstArg).
    P = JSValue::ToString(vm, first_arg)->GetString();
    
    // c. Let k be 2.
    std::size_t k = 2;
    
    // d. Repeat, while k < argCount
    while (k < arg_count) {
      // i. Let nextArg be the k’th argument.
      JSHandle<JSValue> next_arg = argv->GetArg(k - 1);
      
      // ii. Let P be the result of concatenating the previous value of P, the String "," (a comma), and ToString(nextArg).
      P += std::u16string{u","} + std::u16string{JSValue::ToString(vm, next_arg)->GetString()};
      
      // iii. Increase k by 1.
      ++k;
    }
    // e. Let body be the k’th argument.
    body = argv->GetArg(k - 1);
  }
  
  // 6. Let body be ToString(body).
  std::u16string body_str {JSValue::ToString(vm, body)->GetString()};
  
  // 7. If P is not parsable as a FormalParameterListopt then throw a SyntaxError exception.
  // 8. If body is not parsable as FunctionBody then throw a SyntaxError exception.
  ast::Expression* func_expr {nullptr};
  try {
    auto str = u"function (" + P + u") {" + body_str + u"}";
    Parser parser(u"function (" + P + u") {" + body_str + u"}");
    func_expr = parser.ParseFunctionExpression();
  } catch (const utils::Error& error) {
    THROW_SYNTAX_ERROR_AND_RETURN_VALUE(
      vm, u"Wrong arguments for new Function(p1, p2, ..., pn, body)", JSValue{});
  }
  
  // 9. If body is strict mode code (see 10.1.1) then let strict be true, else let strict be false.
  bool strict = false;
  
  // 10. If strict is true, throw any exceptions specified in 13.1 that apply.
  // todo
  if (strict) {
    
  }
  // 11. Return a new Function object created as specified in 13.2 passing P as
  //     the FormalParameterList and body as the FunctionBody.
  //     Pass in the Global Environment as the Scope parameter and strict as the Strict flag.
  return Builtin::InstantiatingFunctionDeclaration(vm, func_expr, vm->GetGlobalEnv(), strict).GetJSValue();
}

}  // namespace builtins
}  // namespace voidjs
