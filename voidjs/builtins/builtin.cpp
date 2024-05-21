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
#include "voidjs/types/object_factory.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_string.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/builtins/js_number.h"
#include "voidjs/builtins/js_math.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

void Builtin::InitializeBuiltinObjects(VM* vm) {
  InitializeBaseObjects(vm);
  InitializeArrayObjects(vm);
  InitializeStringObjects(vm);
  InitializeBooleanObjects(vm);
  InitializeNumberObjects(vm);
  InitializeMathObjects(vm);
  InitializeErrorObjects(vm);
  
  SetPropretiesForBuiltinObjects(vm);
}

void Builtin::InitializeBaseObjects(VM* vm) {
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // Initialize GlobalObject
  // 
  // Unless otherwise specified, the standard built-in properties of the global object have
  // attributes {[[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: true}.
  // 
  // The global object does not have a [[Construct]] internal property;
  // it is not possible to use the global object as a constructor with the new operator.
  // 
  // The global object does not have a [[Call]] internal property;
  // it is not possible to invoke the global object as a function.
  // 
  // The values of the [[Prototype]] and [[Class]]
  // internal properties of the global object are implementation-dependent.
  JSHandle<GlobalObject> global_obj = factory->NewObject(
    GlobalObject::SIZE, JSType::GLOBAL_OBJECT, ObjectClassType::OBJECT,
    JSHandle<JSValue>{vm, JSValue::Null()}, true, false, false).As<GlobalObject>();

  // Initialize Object Prototype
  // 
  // The value of the [[Prototype]] internal property of the Object prototype object is null,
  // the value of the [[Class]] internal property is "Object",
  // and the initial value of the [[Extensible]] internal property is true.
  JSHandle<JSObject> obj_proto = factory->NewObject(
    JSObject::SIZE, JSType::JS_OBJECT, ObjectClassType::OBJECT,
    JSHandle<JSValue>{vm, JSValue::Null()}, true, false, false).As<JSObject>();

  // Initialzie Function Prototype
  // 
  // The value of the [[Prototype]] internal property of the Function prototype object is
  // the standard built-in Object prototype object (15.2.4).
  // 
  // The initial value of the [[Extensible]] internal property of the Function prototype object is true.
  // 
  // The Function prototype object does not have a valueOf property of its own;
  // however, it inherits the valueOf property from the Object prototype Object.
  JSHandle<JSFunction> func_proto = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    obj_proto.As<JSValue>(), true, false, false).As<JSFunction>();

  // Initialize Object Constructor
  // The value of the [[Prototype]] internal property of the Object constructor is
  // the standard built-in Function prototype object.
  // Besides the internal properties and the length property (whose value is 1),
  // the Object constructor has the following properties:
  JSHandle<JSFunction> obj_ctor = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    func_proto.As<JSValue>(), true, true, false).As<JSFunction>();
  
  // Initialize Function Constructor
  // The Function constructor is itself a Function object and its [[Class]] is "Function".
  // The value of the [[Prototype]] internal property of the Function constructor is
  // the standard built-in Function prototype object (15.3.4).
  // The value of the [[Extensible]] internal property of the Function constructor is true.
  JSHandle<JSFunction> func_ctor = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    func_proto.As<JSValue>(), true, true, false).As<JSFunction>();

  
  vm->SetGlobalObject(global_obj);
  
  vm->SetObjectPrototype(obj_proto);
  vm->SetObjectConstructor(obj_ctor);
  
  vm->SetFunctionPrototype(func_proto);
  vm->SetFunctionConstructor(func_ctor);
}

void Builtin::InitializeArrayObjects(VM* vm) {
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // Initialize Array Prototype
  // 
  // The value of the [[Prototype]] internal property of the Array prototype object is
  // the standard built-in Object prototype object (15.2.4).
  // 
  // The Array prototype object is itself an array; its [[Class]] is "Array",
  // and it has a length property (whose initial value is +0) and
  // the special [[DefineOwnProperty]] internal method described in 15.4.5.1.
  JSHandle<JSArray> arr_proto = factory->NewObject(
    JSArray::SIZE, JSType::JS_ARRAY, ObjectClassType::ARRAY,
    vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSArray>();


  // Initialize Array Constructor
  // 
  // The value of the [[Prototype]] internal property of the Array constructor is the Function prototype object (15.3.4).
  // 
  // Besides the internal properties and the length property (whose value is 1),
  // the Array constructor has the following properties:
  JSHandle<JSFunction> arr_ctor = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    vm->GetFunctionPrototype().As<JSValue>(), true, true, false).As<JSFunction>();
  
  vm->SetArrayPrototype(arr_proto);
  vm->SetArrayConstructor(arr_ctor);
}

void Builtin::InitializeStringObjects(VM *vm) {
  ObjectFactory* factory = vm->GetObjectFactory();

  // Initialize String Prototype
  // 
  // The String prototype object is itself a String object (its [[Class]] is "String")
  // whose value is an empty String.
  // 
  // The value of the [[Prototype]] internal property of the String prototype object is
  // the standard built-in Object prototype object (15.2.4).
  JSHandle<JSString> str_proto = factory->NewObject(
    JSString::SIZE, JSType::JS_STRING, ObjectClassType::STRING,
    vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSString>();

  // Initialize String Constructor
  //
  // The value of the [[Prototype]] internal property of the String constructor is
  // the standard built-in Function prototype object (15.3.4).
  JSHandle<JSFunction> str_ctor = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    vm->GetFunctionPrototype().As<JSValue>(), true, true, false).As<JSFunction>();

  vm->SetStringPrototype(str_proto);
  vm->SetStringConstructor(str_ctor);
}

void Builtin::InitializeBooleanObjects(VM* vm) {
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // Initialize Boolean Prototype
  //
  // The Boolean prototype object is itself a Boolean object (its [[Class]] is "Boolean") whose value is false.
  // 
  // The value of the [[Prototype]] internal property of the Boolean prototype object is
  // the standard built-in Object prototype object (15.2.4).
  JSHandle<JSBoolean> bool_proto = factory->NewObject(
    JSBoolean::SIZE, JSType::JS_BOOLEAN, ObjectClassType::BOOLEAN,
    vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSBoolean>();

  // Initialize Boolean Constructor
  //
  // The value of the [[Prototype]] internal property of the Boolean constructor is the Function prototype object (15.3.4).
  JSHandle<JSFunction> bool_ctor = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    vm->GetFunctionPrototype().As<JSValue>(), true, true, false).As<JSFunction>();

  vm->SetBooleanPrototype(bool_proto);
  vm->SetBooleanConstructor(bool_ctor);
}

void Builtin::InitializeNumberObjects(VM* vm) {
  ObjectFactory* factory = vm->GetObjectFactory();

  // Initialize Number Prototype
  // 
  // The Number prototype object is itself a Number object (its [[Class]] is "Number") whose value is +0.
  //
  // The value of the [[Prototype]] internal property of the Number prototype object is
  // the standard built-in Object prototype object (15.2.4).
  //
  // Unless explicitly stated otherwise, the methods of the Number prototype object defined below
  // are not generic and the this value passed to them must be either a Number value or
  // an Object for which the value of the [[Class]] internal property is "Number".
  JSHandle<JSNumber> num_proto = factory->NewObject(
    JSNumber::SIZE, JSType::JS_NUMBER, ObjectClassType::NUMBER,
    vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSNumber>();

  // Initialize Number Constructor
  //
  // The value of the [[Prototype]] internal property of the Number constructor is
  // the Function prototype object (15.3.4).
  JSHandle<JSFunction> num_ctor = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    vm->GetFunctionPrototype().As<JSValue>(), true, true, false).As<JSFunction>();

  vm->SetNumberPrototype(num_proto);
  vm->SetNumberConstructor(num_ctor);
}

void Builtin::InitializeMathObjects(VM* vm) {
  ObjectFactory* factory = vm->GetObjectFactory();
  // Initialize Math Object
  //
  // The Math object is a single object that has some named properties, some of which are functions.
  //
  // The value of the [[Prototype]] internal property of the Math object is the standard built-in Object prototype object (15.2.4).
  // The value of the [[Class]] internal property of the Math object is "Math".
  // 
  // The Math object does not have a [[Construct]] internal property;
  // it is not possible to use the Math object as a constructor with the new operator.
  // 
  // The Math object does not have a [[Call]] internal property; it is not possible to invoke the Math object as a function.
  JSHandle<JSMath> math_obj = factory->NewObject(
    JSMath::SIZE, JSType::JS_MATH, ObjectClassType::MATH,
    vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSMath>();
  
  vm->SetMathObject(math_obj);
}

void Builtin::InitializeErrorObjects(VM* vm) {
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // Initialize Error Prototype
  // The Error prototype object is itself an Error object (its [[Class]] is "Error").
  // The value of the [[Prototype]] internal property of the Error prototype object is
  // the standard built-in Object prototype object (15.2.4).
  JSHandle<JSError> error_proto = factory->NewObject(
    JSError::SIZE, JSType::JS_ERROR, ObjectClassType::ERROR,
    vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSError>();
  
  // Initialize Error Constructor
  // The value of the [[Prototype]] internal property of the Error constructor is
  // the Function prototype object (15.3.4).
  JSHandle<JSFunction> error_ctor = factory->NewObject(
    JSError::SIZE, JSType::JS_ERROR, ObjectClassType::FUNCTION,
    vm->GetFunctionPrototype().As<JSValue>(), true, true, false).As<JSFunction>();

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

// Defined in ECMAScript 5.1 Chapter 13.2
JSHandle<JSFunction> Builtin::InstantiatingFunctionDeclaration(
  VM* vm, ast::AstNode* ast_node, JSHandle<types::LexicalEnvironment> scope, bool strict) {
  ObjectFactory* factory = vm->GetObjectFactory();
  GlobalConstants* constants = vm->GetGlobalConstants();
  
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
  JSHandle<JSFunction> F = factory->NewObject(
    JSFunction::SIZE, JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
    vm->GetFunctionPrototype().As<JSValue>(), true, true, true).As<JSFunction>();
  F->SetCode(ast_node);
  F->SetScope(scope.As<JSValue>());
  
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
  std::size_t len = params.size();
  
  // 15. Call the [[DefineOwnProperty]] internal method of F with arguments "length",
  ///    Property Descriptor {[[Value]]: len, [[Writable]]: false, [[Enumerable]]: false,
  //     [[Configurable]]: false}, and false.
  types::Object::DefineOwnProperty(vm, F, constants->HandledLengthString(),
                                   types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, JSValue{len}}, false, false, false}, false);
  RETURN_HANDLE_IF_HAS_EXCEPTION(vm, JSFunction);
  
  // 16. Let proto be the result of creating a new object as would be constructed by the expression new Object()
  //     where Object is the standard built-in constructor with that name.
  JSHandle<JSObject> proto = types::Object::Construct(
    vm, vm->GetObjectConstructor(),
    vm->GetGlobalConstants()->HandledUndefined(), {}).As<JSObject>();
  
  // 17. Call the [[DefineOwnProperty]] internal method of proto with arguments "constructor",
  ///    Property Descriptor {[[Value]]: F, { [[Writable]]: true, [[Enumerable]]: false,
  //     [[Configurable]]: true}, and false.
  types::Object::DefineOwnProperty(vm, proto, constants->HandledConstructorString(),
                                   types::PropertyDescriptor{vm, F.As<JSValue>(), true, false, true}, false);
  
  // 18. Call the [[DefineOwnProperty]] internal method of F with arguments "prototype",
  //     Property Descriptor {[[Value]]: proto, { [[Writable]]: true, [[Enumerable]]: false,
  //     [[Configurable]]: false}, and false.
  types::Object::DefineOwnProperty(vm, F, constants->HandledPrototypeString(),
                                   types::PropertyDescriptor{vm, proto.As<JSValue>(), true, false, false}, false);
  
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
  SetPropertiesForBaseObjects(vm);
  SetPropertiesForArrayObjectss(vm);
  SetProeprtiesForStringObjects(vm);
  SetPropertiesForMathObjects(vm);
}

void Builtin::SetPropertiesForBaseObjects(VM *vm) {
  JSHandle<GlobalObject> global_obj = vm->GetGlobalObject();
  JSHandle<JSFunction> obj_ctor = vm->GetObjectConstructor();
  JSHandle<JSObject> obj_proto = vm->GetObjectPrototype();
  JSHandle<JSFunction> func_ctor = vm->GetFunctionConstructor();
  JSHandle<JSFunction> func_proto = vm->GetFunctionPrototype();
  ObjectFactory* factory = vm->GetObjectFactory();
  GlobalConstants* constants = vm->GetGlobalConstants();
  
  // Set properties for Global Object
  SetDataProperty(vm, global_obj, constants->HandledObjectString(),
                  obj_ctor.As<JSValue>(), true, false, true);
  SetDataProperty(vm, global_obj, constants->HandledFunctionString(),
                  func_ctor.As<JSValue>(), true, false, true);
  SetDataProperty(vm, global_obj, constants->HandledArrayString(),
                  vm->GetArrayConstructor().As<JSValue>(), true, false, true);
  SetDataProperty(vm, global_obj, constants->HandledStringString(),
                  vm->GetStringConstructor().As<JSValue>(), true, false, true);
  SetDataProperty(vm, global_obj, constants->HandledBooleanString(),
                  vm->GetBooleanConstructor().As<JSValue>(), true, false, true);
  SetDataProperty(vm, global_obj, constants->HandledNumberString(),
                  vm->GetNumberConstructor().As<JSValue>(), true, false, true);
  SetDataProperty(vm, global_obj, constants->HandledMathString(),
                  vm->GetMathObject().As<JSValue>(), true, false, true);
  SetFunctionProperty(vm, global_obj, factory->NewString(u"print"), GlobalObject::Print, false, false, false);

  // Set properties for Object Constructor
  SetDataProperty(vm, obj_ctor, constants->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{1}}, false, false, false);
  SetDataProperty(vm, obj_ctor, constants->HandledPrototypeString(), obj_proto.As<JSValue>(), false, false, false);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"getPrototypeOf"),
                      JSObject::GetPrototypeOf, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"getOwnPropertyDescriptor"),
                      JSObject::GetOwnPropertyDescriptor, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"getOwnPropertyNames"),
                      JSObject::GetOwnPropertyNames, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"create"),
                      JSObject::Create, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"defineProperty"),
                      JSObject::DefineProperty, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"defineProperties"),
                      JSObject::DefineProperties, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"seal"),
                      JSObject::Seal, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"freeze"),
                      JSObject::Freeze, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"preventExtensions"),
                      JSObject::PreventExtensions, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"isSealed"),
                      JSObject::IsSealed, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"isFrozen"),
                      JSObject::IsFrozen, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"isExtensible"),
                      JSObject::IsExtensible, true, false, true);
  SetFunctionProperty(vm, obj_ctor, factory->NewString(u"keys"),
                      JSObject::Keys, true, false, true);

  // Set properties for Object Prototype
  SetDataProperty(vm, obj_proto, constants->HandledConstructorString(),
                  vm->GetObjectConstructor().As<JSValue>(), true, false, true);
  SetFunctionProperty(vm, obj_proto, factory->NewString(u"toString"),
                      JSObject::ToString, true, false, true);
  SetFunctionProperty(vm, obj_proto, factory->NewString(u"toLocaleString"),
                      JSObject::ToLocaleString, true, false, true);
  SetFunctionProperty(vm, obj_proto, factory->NewString(u"valueOf"),
                      JSObject::ValueOf, true, false, true);
  SetFunctionProperty(vm, obj_proto, factory->NewString(u"hasOwnProperty"),
                      JSObject::HasOwnProperty, true, false, true);
  SetFunctionProperty(vm, obj_proto, factory->NewString(u"isPrototypeOf"),
                      JSObject::IsPrototypeOf, true, false, true);
  SetFunctionProperty(vm, obj_proto, factory->NewString(u"propertyIsEnumerable"),
                      JSObject::PropertyIsEnumerable, true, false, true);

  // Set properties for Function Constructor
  SetDataProperty(vm, func_ctor, constants->HandledPrototypeString(),
                  vm->GetFunctionPrototype().As<JSValue>(), false, false, false);
  SetDataProperty(vm, func_ctor, constants->HandledLengthString(),
                  JSHandle<JSValue>{vm, JSValue{1}}, false, false, false);

  // Set properties for Function Prototype
  SetDataProperty(vm, func_proto, constants->HandledLengthString(),
                  JSHandle<JSValue>{vm, JSValue{0}}, false, false, false);
  SetFunctionProperty(vm, func_proto, factory->NewString(u"apply"),
                      JSFunction::Apply, true, false, true);
  SetFunctionProperty(vm, func_proto, factory->NewString(u"call"),
                      JSFunction::Call, true, false, true);
  SetFunctionProperty(vm, func_proto, factory->NewString(u"bind"),
                      JSFunction::Bind, true, false, true);
}

void Builtin::SetPropertiesForArrayObjectss(VM* vm) {
  JSHandle<JSFunction> arr_ctor = vm->GetArrayConstructor();
  JSHandle<JSArray> arr_proto = vm->GetArrayPrototype();
  ObjectFactory* factory = vm->GetObjectFactory();
  GlobalConstants* constants = vm->GetGlobalConstants();

  // Set properties for Array Constructor
  SetDataProperty(vm, arr_ctor, constants->HandledPrototypeString(),
                  arr_proto.As<JSValue>(), false, false, false);
  SetFunctionProperty(vm, arr_ctor, factory->NewString(u"isArray"),
                      JSArray::IsArray, true, false, true);
  
  // Set properties for Array Prototype
  SetDataProperty(vm, arr_proto, constants->HandledConstructorString(),
                  arr_ctor.As<JSValue>(), false, false, false);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"toString"),
                      JSArray::ToString, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"toLocaleString"),
                      JSArray::ToLocaleString, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"concat"),
                      JSArray::Concat, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"join"),
                      JSArray::Join, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"pop"),
                      JSArray::Pop, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"push"),
                      JSArray::Push, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"reverse"),
                      JSArray::Reverse, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"shift"),
                      JSArray::Shift, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"slice"),
                      JSArray::Slice, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"sort"),
                      JSArray::Sort, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"forEach"),
                      JSArray::ForEach, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"map"),
                      JSArray::Map, true, false, true);
  SetFunctionProperty(vm, arr_proto, factory->NewString(u"filter"),
                      JSArray::Filter, true, false, true);
}

void Builtin::SetProeprtiesForStringObjects(VM* vm) {
  JSHandle<JSFunction> str_ctor = vm->GetStringConstructor();
  JSHandle<JSString> str_proto = vm->GetStringPrototype();
  ObjectFactory* factory = vm->GetObjectFactory();
  GlobalConstants* constants = vm->GetGlobalConstants();

  // Set properties for String Prototype
  SetFunctionProperty(vm, str_proto, factory->NewString(u"toString"),
                      JSString::ToString, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"valueOf"),
                      JSString::ValueOf, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"charAt"),
                      JSString::CharAt, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"concat"),
                      JSString::Concat, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"indexOf"),
                      JSString::IndexOf, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"lastIndexOf"),
                      JSString::LastIndexOf, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"slice"),
                      JSString::Slice, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"substring"),
                      JSString::Substring, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"toLowerCase"),
                      JSString::ToLowerCase, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"toUpperCase"),
                      JSString::ToUpperCase, true, false, true);
  SetFunctionProperty(vm, str_proto, factory->NewString(u"trim"),
                      JSString::Trim, true, false, true);
}

void Builtin::SetPropertiesForMathObjects(VM* vm) {
  JSHandle<JSMath> math_obj = vm->GetMathObject();
  ObjectFactory* factory = vm->GetObjectFactory();
  GlobalConstants* constants = vm->GetGlobalConstants();

  // Set properties for Math Object
  SetFunctionProperty(vm, math_obj, factory->NewString(u"abs"),
                      JSMath::Abs, true, false, true);
  SetFunctionProperty(vm, math_obj, factory->NewString(u"acos"),
                      JSMath::Acos, true, false, true);
  SetFunctionProperty(vm, math_obj, factory->NewString(u"asin"),
                      JSMath::Asin, true, false, true);
}

void Builtin::SetDataProperty(VM* vm, JSHandle<types::Object> obj, JSHandle<types::String> prop_name, JSHandle<JSValue> prop_val,
                              bool writable, bool enumerable, bool configurable) {
  auto prop_map = JSHandle<types::PropertyMap>{vm, obj->GetProperties()};
  
  types::PropertyDescriptor desc = types::PropertyDescriptor{vm, prop_val, writable, enumerable, configurable};
  
  obj->SetProperties(types::PropertyMap::SetProperty(vm, prop_map, prop_name, desc).As<JSValue>());
}

void Builtin::SetFunctionProperty(VM* vm, JSHandle<types::Object> obj, JSHandle<types::String> prop_name, InternalFunctionType func,
                                  bool writable, bool enumerable, bool configurable) {
  SetDataProperty(vm, obj, prop_name,
                  vm->GetObjectFactory()->NewInternalFunction(func).As<JSValue>(),
                  writable, enumerable, configurable);
}

}  // namespace builtins
}  // namespace voidjs
