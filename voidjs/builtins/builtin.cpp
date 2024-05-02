#include "voidjs/builtins/builtin.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/interpreter/vm.h"

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
  auto obj_proto = factory->NewObject(JSType::JS_OBJECT, ObjectClassType::OBJECT,
                                      JSValue::Null(), true, false, false)->AsJSObject();

  // Initialzie Function Prototype
  // The value of the [[Prototype]] internal property of the Function prototype object is
  // the standard built-in Object prototype object (15.2.4).
  // The initial value of the [[Extensible]] internal property of the Function prototype object is true.
  // The Function prototype object does not have a valueOf property of its own;
  // however, it inherits the valueOf property from the Object prototype Object.
  auto func_proto = factory->NewObject(JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
                                       JSValue{obj_proto}, true, false, false)->AsJSFunction();

  // Initialize Object Constructor
  // The value of the [[Prototype]] internal property of the Object constructor is
  // the standard built-in Function prototype object.
  // Besides the internal properties and the length property (whose value is 1),
  // the Object constructor has the following properties:
  auto obj_ctor = factory->NewObject(JSType::JS_OBJECT, ObjectClassType::OBJECT,
                                     JSValue{func_proto}, true, true, false)->AsJSObject();
  
  // Initialize Function Constructor
  // The Function constructor is itself a Function object and its [[Class]] is "Function".
  // The value of the [[Prototype]] internal property of the Function constructor is
  // the standard built-in Function prototype object (15.3.4).
  // The value of the [[Extensible]] internal property of the Function constructor is true.
  auto func_ctor = factory->NewObject(JSType::JS_FUNCTION, ObjectClassType::FUNCTION,
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
  auto error_proto = factory->NewObject(JSType::JS_ERROR, ObjectClassType::ERROR,
                                        JSValue{vm->GetObjectPrototype()}, true, false, false)->AsJSError();
  
  // Initialize Error Constructor
  // The value of the [[Prototype]] internal property of the Error constructor is
  // the Function prototype object (15.3.4).
  auto error_ctor = factory->NewObject(JSType::JS_ERROR, ObjectClassType::ERROR,
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

void Builtin::SetPropretiesForBuiltinObjects(VM* vm) {
  auto global_obj = vm->GetGlobalObject();
  auto factory = vm->GetObjectFactory();

  // Set properties for Global Object
  SetDataProperty(vm, global_obj, factory->NewStringFromTable(u"Object"),
                  JSValue(vm->GetObjectConstructor()), true, false, true);
}

void Builtin::SetDataProperty(VM* vm, types::Object* obj, types::String* prop_name, JSValue prop_val,
                              bool writable, bool enumerable, bool configurable) {
  auto props = obj->GetProperties().GetHeapObject()->AsPropertyMap();
  
  auto desc = types::PropertyDescriptor{prop_val, writable, enumerable, configurable};
  
  obj->SetProperties(JSValue(types::PropertyMap::SetProperty(vm, props, prop_name, desc)));
}

void Builtin::SetFunctionProperty(VM* vm, types::Object* obj, types::String* prop_name, InternalFunctionType func) {
  auto props = obj->GetProperties().GetHeapObject()->AsPropertyMap();
  
  auto internal_func = vm->GetObjectFactory()->NewInternalFunction(func);
  
  auto desc = types::PropertyDescriptor{JSValue(internal_func), true, false, true};
  
  obj->SetProperties(JSValue(types::PropertyMap::SetProperty(vm, props, prop_name, desc)));
}

}  // namespace builtins
}  // namespace voidjs
