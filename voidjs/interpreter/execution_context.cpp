#include "voidjs/interpreter/execution_context.h"

#include <functional>

#include "voidjs/ir/program.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/utils/macros.h"

namespace voidjs {

void ExecutionContext::EnterGlobalCode(VM* vm, ast::AstNode* ast_node) {
  // 1. Initialize the execution context using the global code as described in 10.4.1.1.
  auto global_ctx = new ExecutionContext(vm->GetGlobalEnv(), vm->GetGlobalEnv(), vm->GetGlobalObject());
  vm->PushExecutionContext(global_ctx);

  // 2. Perform Declaration Binding Instantiation as described in 10.5 using the global code.
  DeclarationBindingInstantiation(vm, ast_node, std::vector<JSValue>{});
}

void ExecutionContext::EnterFunctionCode(
  VM* vm, ast::AstNode* ast_node, builtins::JSFunction* F, JSValue this_arg, const std::vector<JSValue>& args) {
  // 1. If the function code is strict code, set the ThisBinding to thisArg.
  // 2. Else if thisArg is null or undefined, set the ThisBinding to the global object.
  // 3. Else if Type(thisArg) is not Object, set the ThisBinding to ToObject(thisArg).
  // 4. Else set the ThisBinding to thisArg.
  types::Object* this_binding {nullptr};
  auto strict = std::invoke([=]() {
    if (ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->IsStrict();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->IsStrict();
    }
  });
  if (strict) {
  } else if (this_arg.IsNull() || this_arg.IsUndefined()) {
    this_binding = vm->GetGlobalObject();
  } else if (!this_arg.IsObject()) {
    this_binding = JSValue::ToObject(vm, this_arg);
    RETURN_VOID_IF_HAS_EXCEPTION(vm);
  } else {
    this_binding = this_arg.GetHeapObject()->AsObject();
  }
  
  // 5. Let localEnv be the result of calling NewDeclarativeEnvironment passing the value of the [[Scope]] internal property of F as the argument.
  auto local_env = types::LexicalEnvironment::NewDeclarativeEnvironmentRecord(vm, F->GetScope());
  
  // 6. Set the LexicalEnvironment to localEnv.
  // 7. Set the VariableEnvironment to localEnv.
  auto context = new ExecutionContext(local_env, local_env, this_binding);

  vm->PushExecutionContext(context);
  
  // 8. Let code be the value of F’s [[Code]] internal property.
  // 9. Perform Declaration Binding Instantiation using the function code code and argumentList as described in 10.5.
  DeclarationBindingInstantiation(vm, ast_node, args);
}

void ExecutionContext::DeclarationBindingInstantiation(VM* vm, ast::AstNode* ast_node, const std::vector<JSValue>& args) {
  auto factory = vm->GetObjectFactory();
  
  // 1. Let env be the environment record component of the running execution context’s VariableEnvironment.
  auto env = vm->GetExecutionContext()->GetVariableEnvironment()->GetEnvRec();

  // 2. If code is eval code, then let configurableBindings be true else let configurableBindings be false.
  // todo
  bool configurable_bindings = false;

  // 3. If code is strict mode code, then let strict be true else let strict be false.
  // todo
  auto strict = false;

  // 4. If code is function code, then
  if (ast_node->IsFunctionExpression() || ast_node->IsFunctionDeclaration()) {
    // a. Let func be the function whose [[Call]] internal method initiated execution of code.
    //    Let names be the value of func’s [[FormalParameters]] internal property.
    auto params = std::invoke([=]() {
      if (ast_node->IsFunctionDeclaration()) {
        return ast_node->AsFunctionDeclaration()->GetParameters();
      } else {
        // ast_node must be FunctionExpression
        return ast_node->AsFunctionExpression()->GetParameters();
      }
    });
    std::vector<types::String*> names;
    for (auto param : params) {
      names.push_back(factory->NewStringFromTable(param->AsIdentifier()->GetName()));
    }
    
    // b. Let argCount be the number of elements in args.
    auto arg_count = args.size();
    
    // c. Let n be the number 0.
    std::size_t n = 0;
    
    // d. For each String argName in names, in list order do
    for (auto name : names) {
      // i. Let n be the current value of n plus 1.
      ++n;
      
      // ii. If n is greater than argCount, let v be undefined otherwise let v be the value of the n’th element of args.
      JSValue v = n > arg_count ? JSValue::Undefined() : args[n - 1];
      
      // iii. Let argAlreadyDeclared be the result of calling env’s HasBinding concrete method passing argName as the argument.
      auto arg_already_declared = types::EnvironmentRecord::HasBinding(vm, env, name);
      
      // iv. If argAlreadyDeclared is false, call env’s CreateMutableBinding concrete method passing argName as the argument.
      if (!arg_already_declared) {
        types::EnvironmentRecord::CreateMutableBinding(vm, env, name, configurable_bindings);
      }
      
      // v. Call env’s SetMutableBinding concrete method passing argName, v, and strict as the arguments.
      types::EnvironmentRecord::SetMutableBinding(vm, env, name, v, strict);
      RETURN_VOID_IF_HAS_EXCEPTION(vm);
    }
  }

  // 5. For each FunctionDeclaration f in code, in source text order do
  const auto& func_decls = std::invoke([](ast::AstNode* ast_node) {
    if (ast_node->IsProgram()) {
      return ast_node->AsProgram()->GetFunctionDeclarations();
    } else if ( ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->GetFunctionDeclarations();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->GetFunctionDeclarations();
    }
  }, ast_node);
  
  for (auto func : func_decls) {
    // a. Let fn be the Identifier in FunctionDeclaration f.
    auto fn = func->GetName()->AsIdentifier()->GetName();
    auto fn_str = factory->NewStringFromTable(fn);

    // b. Let fo be the result of instantiating FunctionDeclaration f as described in Clause 13.
    auto fo = builtins::Builtin::InstantiatingFunctionDeclaration(
      vm, func, vm->GetExecutionContext()->GetVariableEnvironment(), strict);

    // c. Let funcAlreadyDeclared be the result of calling env’s HasBinding
    //    concrete method passing fn as the argument.
    auto func_already_declared = types::EnvironmentRecord::HasBinding(vm, env, fn_str);

    // d. If funcAlreadyDeclared is false, call env’s CreateMutableBinding
    //    concrete method passing fn and configurableBindings as the arguments.
    if (!func_already_declared) {
      types::EnvironmentRecord::CreateMutableBinding(vm, env, fn_str, configurable_bindings);
    }
    // e. Else if env is the environment record component of the global environment then
    else if (env == vm->GetGlobalEnv()->GetEnvRec()) {
      // i. Let go be the global object.
      auto go = vm->GetGlobalObject();
      
      // ii. Let existingProp be the resulting of calling the [[GetProperty]] internal method of go with argument fn.
      auto existing_prop = types::Object::GetProperty(vm, go, fn_str);
      
      // iii. If existingProp .[[Configurable]] is true, then
      if (existing_prop.GetConfigurable()) {
        // 1. Call the [[DefineOwnProperty]] internal method of go, passing fn,
        //    Property Descriptor {[[Value]]: undefined, [[Writable]]: true, [[Enumerable]]: true ,
        //    [[Configurable]]: configurableBindings }, and true as arguments.
        types::Object::DefineOwnProperty(vm, go, fn_str, types::PropertyDescriptor{JSValue::Undefined(), true, true, configurable_bindings}, true);
        RETURN_VOID_IF_HAS_EXCEPTION(vm);
      }
      // iv. Else if IsAccessorDescrptor(existingProp) or existingProp does not
      //     have attribute values {[[Writable]]: true, [[Enumerable]]: true}, then
      else if (existing_prop.IsAccessorDescriptor() ||
               !(existing_prop.GetWritable() && existing_prop.GetEnumerable())) {
        // 1. Throw a TypeError exception.
        THROW_TYPE_ERROR_AND_RETURN_VOID(vm, u"DeclarationBindingInstantiation fails.");
      }
    }

    // f. Call env’s SetMutableBinding concrete method passing fn, fo, and strict as the arguments.
    types::EnvironmentRecord::SetMutableBinding(vm, env, fn_str, JSValue{fo}, strict);
  }

  // 6. Let argumentsAlreadyDeclared be the result of
  //    calling env’s HasBinding concrete method passing "arguments" as the argument
  // auto args_already_declared_ = env->HasBinding(ObjectFactory::NewString(u"arguments"));

  // 7. If code is function code and argumentsAlreadyDeclared is false, then
  // todo

  // 8. For each VariableDeclaration and VariableDeclarationNoIn d in code, in source text order do
  const auto& var_decls = std::invoke([](ast::AstNode* ast_node) {
    if (ast_node->IsProgram()) {
      return ast_node->AsProgram()->GetVariableDeclarations();
    } else if ( ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->GetVariableDeclarations();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->GetVariableDeclarations();
    }
  }, ast_node);

  for (auto var_decl : var_decls) {
    // a. Let dn be the Identifier in d.
    auto dn = var_decl->GetIdentifier()->AsIdentifier();
    auto dn_str = factory->NewString(dn->GetName());

    // b. Let varAlreadyDeclared be the result of calling env’s HasBinding concrete method passing dn as the argument.
    auto var_already_declared = types::EnvironmentRecord::HasBinding(vm, env, dn_str);

    // c. If varAlreadyDeclared is false, then
    if (!var_already_declared) {
      // i. Call env’s CreateMutableBinding concrete method
      //    passing dn and configurableBindings as the arguments.
      types::EnvironmentRecord::CreateMutableBinding(vm, env, dn_str, configurable_bindings);

      // ii. Call env’s SetMutableBinding concrete method passing dn, undefined, and strict as the arguments.
      types::EnvironmentRecord::SetMutableBinding(vm, env, dn_str, JSValue::Undefined(), strict);
    }
  }
}

}  // namespace voidjs
