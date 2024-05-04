#include "voidjs/builtins/builtin.h"

#include <functional>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/expression.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

void Builtin::Initialize(VM* vm) {
  InitializeBuiltinObjects(vm);
  SetPropretiesForBuiltinObjects(vm);
}

void Builtin::InitializeBuiltinObjects(VM* vm) {
  auto factory = vm->GetObjectFactory();
  
  // Initialize GlobalObject
  auto global_obj = factory->NewGlobalObject();
  vm->SetGlobalObject(global_obj);

  // Initialize Object Prototype
  // The value of the [[Prototype]] internal property of the Object prototype object is null,
  // the value of the [[Class]] internal property is "Object",
  // and the initial value of the [[Extensible]] internal property is true.
  auto obj_proto = factory->NewObject(JSObject::SIZE, JSType::JS_OBJECT, ObjectClassType::OBJECT,
                                      JSValue::Null(), true, false, false)->AsJSObject();

  // Initialzie Function Prototype
  // The value of the [[Prototype]] internal property of the Function prototype object is
  // the standard built-in Object prototype object (15.2.4).
  // The initial value of the [[Extensible]] internal property of the Function prototype object is true.
  // The Function prototype object does not have a valueOf property of its own;
  // however, it inherits the valueOf property from the Object prototype Object.
  auto func_proto = factory->NewObject(JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
                                       JSValue{obj_proto}, true, false, false)->AsJSFunction();

  // Initialize Object Constructor
  // The value of the [[Prototype]] internal property of the Object constructor is
  // the standard built-in Function prototype object.
  // Besides the internal properties and the length property (whose value is 1),
  // the Object constructor has the following properties:
  auto obj_ctor = factory->NewObject(JSObject::SIZE, JSType::JS_OBJECT, ObjectClassType::FUNCTION,
                                     JSValue{func_proto}, true, true, false)->AsJSObject();
  
  // Initialize Function Constructor
  // The Function constructor is itself a Function object and its [[Class]] is "Function".
  // The value of the [[Prototype]] internal property of the Function constructor is
  // the standard built-in Function prototype object (15.3.4).
  // The value of the [[Extensible]] internal property of the Function constructor is true.
  auto func_ctor = factory->NewObject(JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
                                      JSValue{func_proto}, true, true, false)->AsJSFunction();

  
  vm->SetObjectPrototype(obj_proto);
  vm->SetObjectConstructor(obj_ctor);
  
  vm->SetFunctionPrototype(func_proto);
  vm->SetFunctionConstructor(func_ctor);
}

void Builtin::InitializeErrorObjects(VM* vm) {
  auto factory = vm->GetObjectFactory();
  
  // Initialize Error Prototype
  // The Error prototype object is itself an Error object (its [[Class]] is "Error").
  // The value of the [[Prototype]] internal property of the Error prototype object is
  // the standard built-in Object prototype object (15.2.4).
  auto error_proto = factory->NewObject(JSError::SIZE, JSType::JS_ERROR, ObjectClassType::ERROR,
                                        JSValue{vm->GetObjectPrototype()}, true, false, false)->AsJSError();
  
  // Initialize Error Constructor
  // The value of the [[Prototype]] internal property of the Error constructor is
  // the Function prototype object (15.3.4).
  auto error_ctor = factory->NewObject(JSError::SIZE, JSType::JS_ERROR, ObjectClassType::FUNCTION,
                                       JSValue{vm->GetFunctionPrototype()}, true, false, false)->AsJSError();

  // Initialize Native Error Object, which includes
  // EvalError Prototype, EvalError Constructor, 
  // RangeError Protytpe, RangeError Constructor,
  // ReferenceError Prototype, ReferenceError Constructor,
  // SyntaxError Prototype, SyntaxError Constructor,
  // TypeError Prototype, TypeError Constructor,
  // URIError Prototype, URIError Constructor,
  
  // The value of the [[Prototype]] internal property of each NativeError prototype object is
  // the standard built-in Error prototype object (15.11.4).

  // The value of the [[Prototype]] internal property of a NativeError constructor is
  // the Function prototype object (15.3.4).

  // We use MACROS to define Native Error Object, because they only differ in name.
  INITIALIZE_NATIVE_ERROR(eval_error);
  INITIALIZE_NATIVE_ERROR(range_error);
  INITIALIZE_NATIVE_ERROR(reference_error);
  INITIALIZE_NATIVE_ERROR(syntax_error);
  INITIALIZE_NATIVE_ERROR(type_error);
  INITIALIZE_NATIVE_ERROR(uri_error);

  vm->SetErrorPrototype(error_proto);
  vm->SetErrorConstructor(error_ctor);

  vm->SetEvalErrorPrototype(eval_error_proto);
  vm->SetEvalErrorConstructor(eval_error_ctor);
  
  vm->SetRangeErrorPrototype(range_error_proto);
  vm->SetRangeErrorConstructor(range_error_ctor);
  
  vm->SetReferenceErrorPrototype(reference_error_proto);
  vm->SetReferenceErrorConstructor(reference_error_ctor);
  
  vm->SetSyntaxErrorPrototype(syntax_error_proto);
  vm->SetSyntaxErrorConstructor(syntax_error_ctor);
  
  vm->SetTypeErrorPrototype(type_error_proto);
  vm->SetTypeErrorConstructor(type_error_ctor);
  
  vm->SetURIErrorPrototype(uri_error_proto);
  vm->SetURIErrorConstructor(uri_error_ctor);
}


builtins::JSFunction* Builtin::InstantiatingFunctionDeclaration(
  VM* vm, ast::AstNode* ast_node, types::LexicalEnvironment* scope, bool strict) {
  auto factory = vm->GetObjectFactory();
  
  // 1. Create a new native ECMAScript object and let F be that object.
  // 2. Set all the internal methods, except for [[Get]], of F as described in 8.12.
  // 3. Set the [[Class]] internal property of F to "Function".
  // 4. Set the [[Prototype]] internal property of F to
  //    the standard built-in Function prototype object as specified in 15.3.3.1.
  // 5. Set the [[Get]] internal property of F as described in 15.3.5.4.
  // 6. Set the [[Call]] internal property of F as described in 13.2.1.
  // 7. Set the [[Construct]] internal property of F as described in 13.2.2.
  // 8. Set the [[HasInstance]] internal property of F as described in 15.3.5.3.
  // 9. Set the [[Scope]] internal property of F to the value of Scope.
  // 10. Let names be a List containing, in left to right textual order,
  //     the Strings corresponding to the identifiers of FormalParameterList.
  // 11. Set the [[FormalParameters]] internal property of F to names.
  // 12. Set the [[Code]] internal property of F to FunctionBody.
  // 13. Set the [[Extensible]] internal property of F to true.
  auto F = factory->NewObject(JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
                              JSValue{vm->GetFunctionPrototype()}, true, true, true)->AsJSFunction();
  F->SetCode(ast_node);
  F->SetScope(scope);
  
  // 14. Let len be the number of formal parameters specified in FormalParameterList.
  //     If no parameters are specified, let len be 0.
  auto params = std::invoke([=]() {
    if (ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionExpression()->GetParameters();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->GetParameters();
    }
  });
  auto len = params.size();
  
  // 15. Call the [[DefineOwnProperty]] internal method of F with arguments "length",
  ///    Property Descriptor {[[Value]]: len, [[Writable]]: false, [[Enumerable]]: false,
  //     [[Configurable]]: false}, and false.
  types::Object::DefineOwnProperty(vm, F, factory->NewStringFromTable(u"length"),
                                   types::PropertyDescriptor{JSValue{len}, false, false, false}, false);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, nullptr);
  
  // 16. Let proto be the result of creating a new object as would be constructed by the expression new Object()
  //     where Object is the standard built-in constructor with that name.
  auto proto = JSObject::Construct(factory->NewRuntimeCallInfo(JSValue::Undefined(), std::vector<JSValue>{}));
  
  // 17. Call the [[DefineOwnProperty]] internal method of proto with arguments "constructor",
  ///    Property Descriptor {[[Value]]: F, { [[Writable]]: true, [[Enumerable]]: false,
  //     [[Configurable]]: true}, and false.
  types::Object::DefineOwnProperty(vm, proto, factory->NewStringFromTable(u"constuctor"),
                                   types::PropertyDescriptor{JSValue{F}, true, false, true}, false);
  
  // 18. Call the [[DefineOwnProperty]] internal method of F with arguments "prototype",
  //     Property Descriptor {[[Value]]: proto, { [[Writable]]: true, [[Enumerable]]: false,
  //     [[Configurable]]: false}, and false.
  types::Object::DefineOwnProperty(vm, F, factory->NewStringFromTable(u"prototype"),
                                   types::PropertyDescriptor{JSValue{proto}, true, false, false}, false);
  
  // 19. If Strict is true, then
  if (strict) {
    // a. Let thrower be the [[ThrowTypeError]] function Object (13.2.3).
    // b. Call the [[DefineOwnProperty]] internal method of F with arguments "caller", PropertyDescriptor {[[Get]]: thrower, [[Set]]: thrower, [[Enumerable]]: false, [[Configurable]]: false}, and false.
    // c. Call the [[DefineOwnProperty]] internal method of F with arguments "arguments", PropertyDescriptor {[[Get]]: thrower, [[Set]]: thrower, [[Enumerable]]: false, [[Configurable]]: false}, and false.
    // todo
  }

  // 20. Return F.
  return F;
}

void Builtin::SetPropretiesForBuiltinObjects(VM* vm) {
  auto global_obj = vm->GetGlobalObject();
  auto factory = vm->GetObjectFactory();
  auto obj_ctro = vm->GetObjectConstructor();
  auto obj_proto = vm->GetObjectPrototype();

  // Set properties for Global Object
  SetDataProperty(vm, global_obj, factory->NewStringFromTable(u"Object"),
                  JSValue(vm->GetObjectConstructor()), true, false, true);
  SetDataProperty(vm, global_obj, factory->NewStringFromTable(u"Function"),
                  JSValue(vm->GetFunctionConstructor()), true, false, true);

  // Set propreties for Object Constructor
  SetDataProperty(vm, obj_ctro, factory->NewString(u"length"), JSValue{1}, false, false, false);
  SetDataProperty(vm, obj_ctro, factory->NewString(u"prototype"), JSValue{obj_proto}, false, false, false);
  SetFunctionProperty(vm, obj_ctro, factory->NewStringFromTable(u"getPrototypeOf"),
                      JSObject::GetPrototypeOf, true, false, true);
  SetFunctionProperty(vm, obj_ctro, factory->NewStringFromTable(u"getOwnPropertyDescriptor"),
                      JSObject::GetPrototypeOf, true, false, true);
}

void Builtin::SetDataProperty(VM* vm, types::Object* obj, types::String* prop_name, JSValue prop_val,
                              bool writable, bool enumerable, bool configurable) {
  auto props = obj->GetProperties().GetHeapObject()->AsPropertyMap();
  
  auto desc = types::PropertyDescriptor{prop_val, writable, enumerable, configurable};
  
  obj->SetProperties(JSValue(types::PropertyMap::SetProperty(vm, props, prop_name, desc)));
}

void Builtin::SetFunctionProperty(VM* vm, types::Object* obj, types::String* prop_name, InternalFunctionType func,
                                  bool writable, bool enumerable, bool configurable) {
  SetDataProperty(vm, obj, prop_name,
                  JSValue{vm->GetObjectFactory()->NewInternalFunction(func)},
                  writable, enumerable, configurable);
}

}  // namespace builtins
}  // namespace voidjs
