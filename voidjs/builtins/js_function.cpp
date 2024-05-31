#include "voidjs/builtins/js_function.h"

#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/ir/ast.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// [[HasInstance]](V)
// Defined in ECMAScript 5.1 Chapter 15.3.5.3
bool JSFunction::HasInstance(VM* vm, JSHandle<JSFunction> F, JSHandle<JSValue> V) {
  // 1. If V is not an object, return false.
  if (!V->IsObject()) {
    return false;
  }
  
  // 2. Let O be the result of calling the [[Get]] internal method of F with property name "prototype".
  JSHandle<JSValue> O = types::Object::Get(vm, F, vm->GetGlobalConstants()->HandledPrototypeString());
  
  // 3. If Type(O) is not Object, throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"Constructor has no prototype in HasInstance.", false);
  }
  
  // 4.Repeat
  while (true) {
    // a. Let V be the value of the [[Prototype]] internal property of V.
    V = JSHandle<JSValue>{vm, V.As<types::Object>()->GetPrototype()};
    
    // b. If V is null, return false.
    if (V->IsNull()) {
      return false;
    }
    
    // c. If O and V refer to the same object, return true.
    if (O.GetJSValue() == V.GetJSValue()) {
      return true;
    }
  }
}

// [[Get]](P)
// Defined in ECMAScript 5.1 Chapter 15.3.5.4
JSHandle<JSValue> JSFunction::Get(VM* vm, JSHandle<JSFunction> O, JSHandle<types::String> P) {
  // 1. Let v be the result of calling the default [[Get]] internal method (8.12.3) on F passing P as the property name argument.
  JSHandle<JSValue> v = types::Object::GetDefault(vm, O, P);
  
  // 2. If P is "caller" and v is a strict mode Function object, throw a TypeError exception.
  // todo;
  
  // 3. Return v.
  return v;
}

// Function (p1, p2, ..., pn, body)
// Defined in ECMAScript 5.1 Chapter 15.3.1.1
JSValue JSFunction::FunctionConstructorCall(RuntimeCallInfo* argv) {
  return FunctionConstructorConstruct(argv);
}

// new Function (p1, p2, ..., pn, body)
// Defined in ECMAScript 5.1 Chapter 15.3.2.1
JSValue JSFunction::FunctionConstructorConstruct(RuntimeCallInfo* argv) {
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
  bool strict = func_expr->AsFunctionExpression()->IsStrict();
  
  // 10. If strict is true, throw any exceptions specified in 13.1 that apply.
  // todo
  if (strict) {
    
  }
  // 11. Return a new Function object created as specified in 13.2 passing P as
  //     the FormalParameterList and body as the FunctionBody.
  //     Pass in the Global Environment as the Scope parameter and strict as the Strict flag.
  return Builtin::InstantiatingFunctionDeclaration(vm, func_expr, vm->GetGlobalEnv(), strict).GetJSValue();
}

// Function.prototype.toString()
// Defined in ECMAScript 5.1 Chapter 15.3.4.2
JSValue JSFunction::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = vm->GetObjectFactory();

  return factory->NewString(u"[object Function]").GetJSValue();
}

// Function.prototype.apply(thisArg, argArray)
// Defined in ECMAScript 5.1 Chapter 15.3.4.3
JSValue JSFunction::Apply(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> this_arg = argv->GetArg(0);
  JSHandle<JSValue> arg_array = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. If IsCallable(func) is false, then throw a TypeError exception.
  if (!this_value->IsObject() || !this_value->GetHeapObject()->GetCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"Function is not callable when using Function.prototype.apply.", JSValue{});
  }
  
  // 2. If argArray is null or undefined, then
  if (arg_array->IsUndefined() || arg_array->IsNull()) {
    // a. Return the result of calling the [[Call]] internal method of func,
    //    providing thisArg as the this value and an empty list of arguments.
    return types::Object::Call(vm, this_value.As<types::Object>(), this_arg, {}).GetJSValue();
  }
  
  // 3. If Type(argArray) is not Object, then throw a TypeError exception.
  if (!arg_array->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"Apply requires the second argument to be an object.", JSValue{});
  }
  
  // 4. Let len be the result of calling the [[Get]] internal method of argArray with argument "length".
  JSHandle<JSValue> len = types::Object::Get(vm, arg_array.As<types::Object>(), vm->GetGlobalConstants()->HandledLengthString());
  
  // 5. Let n be ToUint32(len).
  std::uint32_t n = JSValue::ToUint32(vm, len);
  
  // 6. Let argList be an empty List.
  std::vector<JSHandle<JSValue>> arg_list;
  
  // 7. Let index be 0.
  std::uint32_t index = 0;
  
  // 8. Repeat while index < n
  while (index < n) {
    // a. Let indexName be ToString(index).
    JSHandle<types::String> index_name = factory->NewStringFromInt(index);
    
    // b. Let nextArg be the result of calling the [[Get]] internal method of argArray with indexName as the argument.
    JSHandle<JSValue> next_arg = types::Object::Get(vm, arg_array.As<types::Object>(), index_name);
    
    // c. Append nextArg as the last element of argList.
    arg_list.push_back(next_arg);
    
    // d. Set index to index + 1.
    ++index;
  }
  
  // 9. Return the result of calling the [[Call]] internal method of func, providing thisArg as the this value and argList as the list of arguments.
  return types::Object::Call(vm, this_value.As<types::Object>(), this_arg, arg_list).GetJSValue();
}

// Function.prototype.call(thisArg, [ , arg1 [ , arg2, ... ] ])
// Defined in ECMAScript 5.1 Chapter 15.3.4.4
JSValue JSFunction::Call(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> this_arg = argv->GetArg(0);
  std::uint64_t args_num = argv->GetArgsNum();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. If IsCallable(func) is false, then throw a TypeError exception.
  if (!this_value->IsObject() || !this_value->GetHeapObject()->GetCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"Function is not callable when using Function.prototype.call.", JSValue{});
  }
  
  // 2. Let argList be an empty List.
  std::vector<JSHandle<JSValue>> arg_list;
  
  // 3. If this method was called with more than one argument then in left to right order starting with arg1 append each argument as the last element of argList
  for (std::size_t idx = 1; idx < args_num; ++idx) {
    arg_list.push_back(argv->GetArg(idx));
  }
  
  // 4. Return the result of calling the [[Call]] internal method of func, providing thisArg as the this value and argList as the list of arguments.
  return types::Object::Call(vm, this_value.As<types::Object>(), this_arg, arg_list).GetJSValue();
}

JSValue JSFunction::Bind(RuntimeCallInfo* argv) {
  return {};
}

}  // namespace builtins
}  // namespace voidjs
