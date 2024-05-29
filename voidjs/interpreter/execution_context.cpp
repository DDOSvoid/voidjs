#include "voidjs/interpreter/execution_context.h"

#include <functional>

#include "voidjs/ir/program.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/arguments.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/macros.h"

namespace voidjs {

void ExecutionContext::EnterGlobalCode(VM* vm, ast::AstNode* ast_node, bool is_strict) {
  // 1. Initialize the execution context using the global code as described in 10.4.1.1.
  auto global_ctx = new ExecutionContext(vm->GetGlobalEnv(), vm->GetGlobalEnv(), vm->GetGlobalObject(), is_strict);
  vm->PushExecutionContext(global_ctx);

  // 2. Perform Declaration Binding Instantiation as described in 10.5 using the global code.
  DeclarationBindingInstantiation(vm, ast_node, {}, {});
}

void ExecutionContext::EnterFunctionCode(
  VM* vm, ast::AstNode* ast_node, JSHandle<builtins::JSFunction> F, JSHandle<JSValue> this_arg, const std::vector<JSHandle<JSValue>>& args) {
  // 1. If the function code is strict code, set the ThisBinding to thisArg.
  // 2. Else if thisArg is null or undefined, set the ThisBinding to the global object.
  // 3. Else if Type(thisArg) is not Object, set the ThisBinding to ToObject(thisArg).
  // 4. Else set the ThisBinding to thisArg.
  JSHandle<types::Object> this_binding;
  bool strict = std::invoke([=]() {
    if (ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->IsStrict();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->IsStrict();
    }
  });
  if (strict) {
    this_binding = this_arg.As<types::Object>();
  } else if (this_arg->IsNull() || this_arg->IsUndefined()) {
    this_binding = vm->GetGlobalObject();
  } else if (!this_arg->IsObject()) {
    this_binding = JSValue::ToObject(vm, this_arg);
    RETURN_VOID_IF_HAS_EXCEPTION(vm);
  } else {
    this_binding = this_arg.As<types::Object>();
  }
  
  // 5. Let localEnv be the result of calling NewDeclarativeEnvironment passing the value of the [[Scope]] internal property of F as the argument.
  auto local_env = types::LexicalEnvironment::NewDeclarativeEnvironmentRecord(vm, JSHandle<types::LexicalEnvironment>{vm, F->GetScope()});
  
  // 6. Set the LexicalEnvironment to localEnv.
  // 7. Set the VariableEnvironment to localEnv.
  auto context = new ExecutionContext(local_env, local_env, this_binding, strict);

  vm->PushExecutionContext(context);
  
  // 8. Let code be the value of F’s [[Code]] internal property.
  // 9. Perform Declaration Binding Instantiation using the function code code and argumentList as described in 10.5.
  DeclarationBindingInstantiation(vm, ast_node, F, args);
}

void ExecutionContext::DeclarationBindingInstantiation(VM* vm, ast::AstNode* ast_node, JSHandle<builtins::JSFunction> F, const std::vector<JSHandle<JSValue>>& args) {
  auto factory = vm->GetObjectFactory();
  
  // 1. Let env be the environment record component of the running execution context’s VariableEnvironment.
  auto env = JSHandle<types::EnvironmentRecord>{vm, vm->GetExecutionContext()->GetVariableEnvironment()->GetEnvRec()};

  // 2. If code is eval code, then let configurableBindings be true else let configurableBindings be false.
  // todo
  bool configurable_bindings = false;

  // 3. If code is strict mode code, then let strict be true else let strict be false.
  bool strict = std::invoke([=]() {
    if (ast_node->IsProgram()) {
      return ast_node->AsProgram()->IsStrict();
    } else if (ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->IsStrict();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->IsStrict();
    }
  });

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
    std::vector<JSHandle<types::String>> names;
    for (auto param : params) {
      names.push_back(factory->NewString(param->AsIdentifier()->GetName()));
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
      auto v = n > arg_count ? JSHandle<JSValue>{vm, JSValue::Undefined()} : args[n - 1];
      
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
    auto fn_str = factory->NewString(fn);

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
    else if (env.GetJSValue().GetRawData() == vm->GetGlobalEnv()->GetEnvRec().GetRawData()) {
      // i. Let go be the global object.
      auto go = vm->GetGlobalObject();
      
      // ii. Let existingProp be the resulting of calling the [[GetProperty]] internal method of go with argument fn.
      auto existing_prop = types::Object::GetProperty(vm, go, fn_str);
      
      // iii. If existingProp .[[Configurable]] is true, then
      if (existing_prop.GetConfigurable()) {
        // 1. Call the [[DefineOwnProperty]] internal method of go, passing fn,
        //    Property Descriptor {[[Value]]: undefined, [[Writable]]: true, [[Enumerable]]: true ,
        //    [[Configurable]]: configurableBindings }, and true as arguments.
        types::Object::DefineOwnProperty(vm, go, fn_str, types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, JSValue::Undefined()}, true, true, configurable_bindings}, true);
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
    types::EnvironmentRecord::SetMutableBinding(vm, env, fn_str, fo.As<JSValue>(), strict);
  }

  // 6. Let argumentsAlreadyDeclared be the result of
  //    calling env’s HasBinding concrete method passing "arguments" as the argument
  bool arguments_already_declared = types::EnvironmentRecord::HasBinding(vm, env, factory->NewString(u"arguments"));

  // 7. If code is function code and argumentsAlreadyDeclared is false, then
  if ((ast_node->IsFunctionDeclaration() || ast_node->IsFunctionExpression()) && !arguments_already_declared) {
    // a. Let argsObj be the result of calling the abstract operation
    //    CreateArgumentsObject (10.6) passing func, names, args, env and strict as arguments.
    JSHandle<builtins::Arguments> args_obj = CreateArgumentsObject(vm, ast_node, F, args, env, strict);

    JSHandle<types::String> arguments_string = factory->NewString(u"arguments");
    // b. If strict is true, then
    if (strict) {
      // i. Call env’s CreateImmutableBinding concrete method passing the String "arguments" as the argument.
      types::DeclarativeEnvironmentRecord::CreateImmutableBinding(vm, env.As<types::DeclarativeEnvironmentRecord>(), arguments_string);
                                                                  
      // ii. Call env’s InitializeImmutableBinding concrete method passing "arguments" and argsObj as arguments.
      types::DeclarativeEnvironmentRecord::InitializeImmutableBinding(
        vm, env.As<types::DeclarativeEnvironmentRecord>(), arguments_string, args_obj.As<JSValue>());
    }
    // c. Else,
    else {
      // i. Call env’s CreateMutableBinding concrete method passing the String "arguments" as the argument.
      types::EnvironmentRecord::CreateMutableBinding(vm, env, arguments_string, false);
      
      // ii. Call env’s SetMutableBinding concrete method passing "arguments", argsObj, and false as arguments.
      types::EnvironmentRecord::SetMutableBinding(vm, env, arguments_string, args_obj.As<JSValue>(), false);
    }
  }

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
      types::EnvironmentRecord::SetMutableBinding(vm, env, dn_str, JSHandle<JSValue>{vm, JSValue::Undefined()}, strict);
    }
  }
}

JSHandle<builtins::Arguments> ExecutionContext::CreateArgumentsObject(
  VM* vm, ast::AstNode* ast_node, JSHandle<builtins::JSFunction> F,
  const std::vector<JSHandle<JSValue>>& args, JSHandle<types::EnvironmentRecord> env, bool strict) {
  ObjectFactory* factory = vm->GetObjectFactory();

  auto params = std::invoke([=]() {
    if (ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->GetParameters();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->GetParameters();
    }
  });
  std::vector<JSHandle<types::String>> names;
  for (auto param : params) {
    names.push_back(factory->NewString(param->AsIdentifier()->GetName()));
  }
  
  // 1. Let len be the number of elements in args.
  int len = args.size();
  
  // 2. Let obj be the result of creating a new ECMAScript object.
  // 3. Set all the internal methods of obj as specified in 8.12.
  // 4. Set the [[Class]] internal property of obj to "Arguments".
  // 5. Let Object be the standard built-in Object constructor (15.2.2).
  // 6. Set the [[Prototype]] internal property of obj to the standard built-in Object prototype object (15.2.4).
  JSHandle<builtins::Arguments> obj =
    factory->NewObject(builtins::Arguments::SIZE, JSType::ARGUMENTS, ObjectClassType::ARGUMENTS,
                       vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<builtins::Arguments>();
  
  // 7. Call the [[DefineOwnProperty]] internal method on obj passing "length",
  //    the Property Descriptor {[[Value]]: len, [[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: true}, and false as arguments.
  types::Object::DefineOwnProperty(vm, obj, vm->GetGlobalConstants()->HandledLengthString(),
                                   types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, types::Number{len}}, true, false, true}, false);
  
  // 8. Let map be the result of creating a new object as if by the expression new Object() where Object is the standard built-in constructor with that name
  JSHandle<builtins::JSObject> map =
    types::Object::Construct(vm, vm->GetObjectConstructor(), vm->GetGlobalConstants()->HandledUndefined(), {}).As<builtins::JSObject>();
  
  // 9. Let mappedNames be an empty List.
  std::vector<JSHandle<types::String>> mapped_names;
  
  // 10. Let indx = len - 1.
  int indx = len - 1;
  
  // 11. Repeat while indx >= 0,
  while (indx >= 0) {
    // a. Let val be the element of args at 0-origined list position indx.
    JSHandle<JSValue> val = args[indx];
    
    // b. Call the [[DefineOwnProperty]] internal method on obj passing ToString(indx),
    //    the property descriptor {[[Value]]: val, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false as arguments.
    types::Object::DefineOwnProperty(vm, obj, factory->NewStringFromInt(indx),
                                     types::PropertyDescriptor{vm, val, true, true, true}, false);

    // c. If indx is less than the number of elements in names, then
    // todo
    if (indx < names.size()) {
      // i. Let name be the element of names at 0-origined list position indx.
      JSHandle<types::String> name = names[indx];
      
      // ii. If strict is false and name is not an element of mappedNames, then
      if (!strict) {
        // a. Add name as an element of the list mappedNames.
        mapped_names.push_back(name);

        // todo
        // b. Let g be the result of calling the MakeArgGetter abstract operation with arguments name and env.
        // JSHandle<builtins::JSFunction> g;
        {
          // 1. Let body be the result of concatenating the Strings "return ", name, and ";"
          // JSHandle<types::String> body = types::String::Concat(vm, factory->NewString(u"return "), name, factory->NewString(u";"));
          
          // 2. Return the result of creating a function object as described in 13.2
          //    using no FormalParameterList, body for FunctionBody, env as Scope, and true for Strict.
          // g = builtins::Builtin::InstantiatingFunctionDeclaration(vm, ast_node, env, true);
        }
        
        // c. Let p be the result of calling the MakeArgSetter abstract operation with arguments name and env.
        // JSHandle<builtins::JSFunction> f;
        {
          // 1. Let param be the String name concatenated with the String "_arg"
          // 2. Let body be the String "<name> = <param>;" with <name> replaced by the value of name and <param> replaced by the value of param.
          // 3. Return the result of creating a function object as described in 13.2
          //    using a List containing the single String param as FormalParameterList, body for FunctionBody, env as Scope, and true for Strict.
        }
        
        // d. Call the [[DefineOwnProperty]] internal method of map passing ToString(indx),
        //    the Property Descriptor {[[Set]]: p, [[Get]]: g, [[Configurable]]: true}, and false as arguments.
        
      }
    }

    // d. Let indx = indx - 1
    --indx;
  }
  
  // 12. If mappedNames is not empty, then
  if (!mapped_names.empty()) {
    // a. Set the [[ParameterMap]] internal property of obj to map.
    obj->SetParameterMap(map.As<JSValue>());
    
    // b. Set the [[Get]], [[GetOwnProperty]], [[DefineOwnProperty]], and [[Delete]] internal methods of obj to the definitions provided below.
    
  }

  // 13. If strict is false, then
  if (!strict) {
    // a. Call the [[DefineOwnProperty]] internal method on obj passing "callee",
    //    the property descriptor {[[Value]]: func, [[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: true}, and false as arguments.
    types::Object::DefineOwnProperty(vm, obj, factory->NewString(u"callee"),
                                     types::PropertyDescriptor{vm, F.As<JSValue>(), true, false, true}, false);
  }
  // 14. Else, strict is true so
  else {
    // todo
    // a. Let thrower be the [[ThrowTypeError]] function Object (13.2.3).
    // b. Call the [[DefineOwnProperty]] internal method of obj with arguments "caller",
    //    PropertyDescriptor {[[Get]]: thrower, [[Set]]: thrower, [[Enumerable]]: false, [[Configurable]]: false}, and false.
    // c. Call the [[DefineOwnProperty]] internal method of obj with arguments "callee",
    //    PropertyDescriptor {[[Get]]: thrower, [[Set]]: thrower, [[Enumerable]]: false, [[Configurable]]: false}, and false.
  }

  // 15. Return obj.
  return obj;
}

}  // namespace voidjs
