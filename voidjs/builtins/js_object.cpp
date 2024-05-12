#include "voidjs/builtins/js_object.h"

#include "voidjs/builtins/js_array.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// new Object ( [ value ] )
// Defined in ECMAScript 5.1 Chapter 15.2.2.1
JSValue JSObject::ObjectConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> value = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. If value is supplied, then
  if (!value.IsEmpty() && !value->IsUndefined() && !value->IsNull()) {
    // a .If Type(value) is Object, then
    if (value->IsObject()) {
      // i. If the value is a native ECMAScript object, do not create a new object but simply return value.
      if (value->GetHeapObject()->IsJSObject()) {
        return value.GetJSValue();
      }
      
      // ii. If the value is a host object, then actions are taken and
      //     a result is returned in an implementation-dependent manner that may depend on the host object.
      // todo
    }
    
    // b. If Type(value) is String, return ToObject(value).
    // c. If Type(value) is Boolean, return ToObject(value).
    // d. If Type(value) is Number, return ToObject(value).
    if (value->IsString() || value->IsBoolean() || value->IsNumber()) {
      return JSValue::ToObject(vm, value).GetJSValue();
    }
  }

  // 2. Assert: The argument value was not supplied or its type was Null or Undefined.
  
  // 3. Let obj be a newly created native ECMAScript object.
  // 4. Set the [[Prototype]] internal property of obj t to the standard built-in Object prototype object (15.2.4).
  // 5 .Set the [[Class]] internal property of obj to "Object".
  // 6. Set the [[Extensible]] internal property of obj to true.
  // 7. Set the all the internal methods of obj as specified in 8.12
  JSHandle<JSObject> obj = factory->NewObject(JSObject::SIZE, JSType::JS_OBJECT, ObjectClassType::OBJECT,
                                             vm->GetObjectPrototype().As<JSValue>(), true, false, false).As<JSObject>();
  
  // 8. Return obj.
  return obj.GetJSValue();
}

// Object([value])
// Defined in ECMAScript 5.1 Chapter 15.2.1.1
JSValue JSObject::ObjectConstructorCall(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> value = argv->GetArg(0);
  
  // 1. If value is null, undefined or not supplied,
  //    create and return a new Object object exactly as if
  //    the standard built-in Object constructor had been called with the same arguments (15.2.2.1).
  if (value.IsEmpty() || value->IsNull() || value->IsUndefined()) {
    return ObjectConstructorConstruct(argv);
  }

  // 2. Return ToObject(value).
  return JSValue::ToObject(vm, value).GetJSValue();
}

// Object.getPrototypeOf(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.2
JSValue JSObject::GetPrototypeOf(RuntimeCallInfo* argv) {
  JSHandle<JSValue> O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.getPrototypeOf cannot work on non-Object type.", JSValue{});
  }
  
  // 2. Return the value of the [[Prototype]] internal property of O.
  return O->GetHeapObject()->AsObject()->GetPrototype();
}

// Object.getOwnPropertyDescriptor(O, P)
// Defined in ECMAScript 5.1 Chapter 15.2.3.3
JSValue JSObject::GetOwnPropertyDescriptor(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);
  JSHandle<JSValue> P = argv->GetArg(1);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.getownPropretyDescriptor cannot work on non-object type.", JSValue{});
  }
  
  // 2. Let name be ToString(P).
  JSHandle<types::String> name = JSValue::ToString(vm, P);
  
  // 3. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with argument name.
  types::PropertyDescriptor desc = Object::GetOwnProperty(vm, O.As<Object>(), name);
  
  // 4. Return the result of calling FromPropertyDescriptor(desc) (8.10.4).
  return desc.FromPropertyDescriptor().GetJSValue();
}

// Object.getOwnPropertyNames(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.4
// todo
JSValue JSObject::GetOwnPropertyNames(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.getownPropretyDescriptor cannot work on non-object type.", JSValue{});
  }
  // 2. Let array be the result of creating a new object as if by the expression new Array() where Array is the standard built-in constructor with that name.
  JSHandle<JSArray> array = types::Object::Construct(vm, vm->GetArrayConstructor(),
                                                     vm->GetGlobalConstants()->HandledUndefined(), {}).As<JSArray>();
  
  // 3. Let n be 0.
  std::int32_t n = 0;
  
  // 4. For each named own property P of O
  auto prop_map = JSHandle<types::PropertyMap>{vm, O.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = prop_map->GetAllKeys(vm);
  for (auto name : keys) {
    // a. Let name be the String value that is the name of P.
    // b. Call the [[DefineOwnProperty]] internal method of array with arguments ToString(n),
    //    the PropertyDescriptor {[[Value]]: name, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    JSArray::DefineOwnProperty(vm, array, vm->GetObjectFactory()->NewStringFromInt(n),
                               types::PropertyDescriptor{vm, name, true, true, true}, false);
    
    // c. Increment n by 1.
    ++n;
  }
  // 5. Return array.
  return array.GetJSValue();
}

// Object.create(O, [, Properties])
// Defined in ECMAScript 5.1 Chapter 15.2.3.5
JSValue JSObject::Create(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = argv->GetVM()->GetObjectFactory();
  JSHandle<JSValue> O = argv->GetArg(0);
  JSHandle<JSValue> Properties = argv->GetArg(1);
  
  // 1. If Type(O) is not Object or Null throw a TypeError exception.
  if (!O->IsObject() && !O->IsNull()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.create cannot work on non-Object type.", JSValue{});
  }
  
  // 2. Let obj be the result of creating a new object as if
  //    by the expression new Object() where Object is the standard built-in constructor with that name
  JSHandle<JSObject> obj = types::Object::Construct(
    vm, vm->GetObjectConstructor(), 
    vm->GetGlobalConstants()->HandledUndefined(), {}).As<JSObject>();
  
  // 3. Set the [[Prototype]] internal property of obj to O.
  obj->SetPrototype(O);
  
  // 4. If the argument Properties is present and not undefined,
  //    add own properties to obj as if by calling the standard built-in function
  //    Object.defineProperties with arguments obj and Properties.
  if (!Properties->IsUndefined()) {
    // Object.DefineProperties(O, Properties)
    
    auto props = JSValue::ToObject(vm, Properties);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});

    auto prop_map = JSHandle<types::PropertyMap>{vm, props.As<types::Object>()->GetProperties()};
    std::vector<JSHandle<JSValue>> keys = prop_map->GetAllEnumerableKeys(vm);
    for (auto key : keys) {
      JSHandle<JSValue> prop = types::Object::Get(vm, props, key.As<types::String>());
    
      auto desc = types::PropertyDescriptor::ToPropertyDescriptor(vm, prop);

      types::Object::DefineOwnProperty(vm, obj, key.As<types::String>(), desc, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }
  }
  
  // 5. Return obj.
  return obj.GetJSValue();
}

// Object.defineProperty(O, P, Attributes)
// Defined in ECMAScript 5.1 Chapter 15.2.3.6
JSValue JSObject::DefineProperty(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);
  JSHandle<JSValue> P = argv->GetArg(1);
  JSHandle<JSValue> Attributes = argv->GetArg(2);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.defineProperty cannot work on non-Object type.", JSValue{});
  }
  
  // 2. Let name be ToString(P).
  JSHandle<types::String> name = JSValue::ToString(vm, P);
  
  // 3. Let desc be the result of calling ToPropertyDescriptor with Attributes as the argument.
  const auto& desc = types::PropertyDescriptor::ToPropertyDescriptor(vm, Attributes);
  
  // 4. Call the [[DefineOwnProperty]] internal method of O with arguments name, desc, and true.
  Object::DefineOwnProperty(vm, O.As<types::Object>(), name, desc, true);
  
  // 5. Return O.
  return O.GetJSValue();
}

// Object.defineProperties(O, Properties)
// Defined in ECMAScript 5.1 Chapter 15.2.3.7
JSValue JSObject::DefineProperties(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);
  JSHandle<JSValue> Properties = argv->GetArg(1);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.defineProperties cannot work on non-Object type.", JSValue{});
  }
  
  // 2. Let props be ToObject(Properties).
  auto props = JSValue::ToObject(vm, Properties);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 3. Let names be an internal list containing the names of each enumerable own property of props.
  // 4. Let descriptors be an empty internal List.
  // 5. For each element P of names in list order,
  // a. Let descObj be the result of calling the [[Get]] internal method of props with P as the argument.
  // b. Let desc be the result of calling ToPropertyDescriptor with descObj as the argument.
  // c. Append desc to the end of descriptors.
  // 6. For each element desc of descriptors in list order,
  // 7. Call the [[DefineOwnProperty]] internal method of O with arguments P, desc, and true.
  // 8. Return O

  auto prop_map = JSHandle<types::PropertyMap>{vm, props.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = prop_map->GetAllEnumerableKeys(vm);
  for (auto key : keys) {
    JSHandle<JSValue> prop = types::Object::Get(vm, props, key.As<types::String>());
    
    auto desc = types::PropertyDescriptor::ToPropertyDescriptor(vm, prop);

    types::Object::DefineOwnProperty(vm, O.As<types::Object>(), key.As<types::String>(), desc, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  }
  
  return O.GetJSValue();
}

// Object.seal(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.8
JSValue JSObject::Seal(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);

  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.seal cannot work on non-Object type.", JSValue{});
  }

  // 2. For each named own property name P of O,
  auto props = JSHandle<types::PropertyMap>{vm, O.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = props->GetAllKeys(vm);
  for (auto key : keys) {
    // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.
    types::PropertyDescriptor desc = types::Object::GetOwnProperty(vm, O.As<types::Object>(), key.As<types::String>());
    
    // b. If desc.[[Configurable]] is true, set desc.[[Configurable]] to false.
    if (desc.GetConfigurable()) {
      desc.SetConfigurable(false);
    }
    
    // c. Call the [[DefineOwnProperty]] internal method of O with P, desc, and true as arguments.
    types::Object::DefineOwnProperty(vm, O.As<types::Object>(), key.As<types::String>(), desc, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  }

  // 3. Set the [[Extensible]] internal property of O to false.
  O.As<types::Object>()->SetExtensible(false);

  // 4. Return O.
  return O.GetJSValue();
}

// Object.freeze(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.9
// todo
JSValue JSObject::Freeze(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.freeze cannot work on non-Object type.", JSValue{});
  }

  // 2. For each named own property name P of O,
  auto props = JSHandle<types::PropertyMap>{vm, O.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = props->GetAllKeys(vm);
  for (auto key : keys) {
    // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.
    types::PropertyDescriptor desc = types::Object::GetOwnProperty(vm, O.As<types::Object>(), key.As<types::String>());
    
    // b. If IsDataDescriptor(desc) is true, then
    if (desc.IsDataDescriptor()) {
      // i.If desc.[[Writable]] is true, set desc.[[Writable]] to false.
      if (desc.GetWritable()) {
        desc.SetWritable(false);
      }
    }
    
    // c. If desc.[[Configurable]] is true, set desc.[[Configurable]] to false.
    if (desc.GetConfigurable()) {
      desc.SetConfigurable(false);
    }

    // d. Call the [[DefineOwnProperty]] internal method of O with P, desc, and true as arguments.
    types::Object::DefineOwnProperty(vm, O.As<types::Object>(), key.As<types::String>(), desc, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  }

  // 3. Set the [[Extensible]] internal property of O to false.
  O.As<types::Object>()->SetExtensible(false);

  // 4. Return O.
  return O.GetJSValue();
}

// Object.preventExtensions(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.10
JSValue JSObject::PreventExtensions(RuntimeCallInfo* argv) {
  JSHandle<JSValue> O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.preventExtensions cannot work on non-Object type.", JSValue{});
  }

  // 2. Set the [[Extensible]] internal property of O to false.
  O.As<HeapObject>()->SetExtensible(false);

  // 3. Return O.
  return O.GetJSValue();
}

// Object.isSealed(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.11
JSValue JSObject::IsSealed(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);

  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.isSealed cannot work on non-Object type.", JSValue{});
  }

  // 2. For each named own property name P of O,
  auto props = JSHandle<types::PropertyMap>{vm, O.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = props->GetAllKeys(vm);
  for (auto key : keys) {
    // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.
    types::PropertyDescriptor desc = types::Object::GetOwnProperty(vm, O.As<types::Object>(), key.As<types::String>());

    // b. If desc.[[Configurable]] is true, then return false.
    if (desc.GetConfigurable()) {
      return JSValue::False();
    }
  }

  // 3. If the [[Extensible]] internal property of O is false, then return true.
  if (!O.As<types::Object>()->GetExtensible()) {
    return JSValue::True();
  }
  // 4. Otherwise, return false.
  else {
    return JSValue::False();
  }
}

// Object.isFrozen
// Defined in ECMAScript 5.1 Chapter 15.2.3.12
JSValue JSObject::IsFrozen(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);

  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.isFrozen cannot work on non-Object type.", JSValue{});
  }

  // 2. For each named own property name P of O,
  auto props = JSHandle<types::PropertyMap>{vm, O.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = props->GetAllKeys(vm);
  for (auto key : keys) {
    // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.
    types::PropertyDescriptor desc = types::Object::GetOwnProperty(vm, O.As<types::Object>(), key.As<types::String>());
    
    // b. If IsDataDescriptor(desc) is true then
    if (desc.IsDataDescriptor()) {
      // i. If desc.[[Writable]] is true, return false.
      if (desc.GetWritable()) {
        return JSValue::False();
      }
    }
    
    // c. If desc.[[Configurable]] is true, then return false.
    if (desc.GetConfigurable()) {
      return JSValue::False();
    }
  }

  // 3. If the [[Extensible]] internal property of O is false, then return true.
  if (!O.As<types::Object>()->GetExtensible()) {
    return JSValue::True();
  }
  // 4. Otherwise, return false.
  else {
    return JSValue::False();
  }
}

// Object.isExtensible
// Defined in ECMAScript 5.1 Chatper 15.2.3.13
JSValue JSObject::IsExtensible(RuntimeCallInfo* argv) {
  JSHandle<JSValue> O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.isExtensible cannot work on non-Object type.", JSValue{});
  }

  // 2. Return the Boolean value of the [[Extensible]] internal property of O.
  return JSValue{O.As<HeapObject>()->GetExtensible()};
}

// Object.keys
// Defined in ECMAScript 5.1 Chapter 15.2.3.14
JSValue JSObject::Keys(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> O = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. If the Type(O) is not Object, throw a TypeError exception.
  if (!O->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      argv->GetVM(), u"Object.keys cannot work on non-Object type.", JSValue{});
  }

  // 2. Let n be the number of own enumerable properties of O
  std::int32_t n = 0; 

  // 3. Let array be the result of creating a new Object as if by the expression new Array(n) where Array is the standard built-in constructor with that name.
  JSHandle<JSArray> array = types::Object::Construct(
    vm, vm->GetArrayConstructor(),
    vm->GetGlobalConstants()->HandledUndefined(), {JSHandle<JSValue>{vm, JSValue{n}}}).As<JSArray>();
  
  // 4. Let index be 0.
  std::int32_t index = 0;

  // 5. For each own enumerable property of O whose name String is P
  auto props = JSHandle<types::PropertyMap>{vm, O.As<types::Object>()->GetProperties()};
  std::vector<JSHandle<JSValue>> keys = props->GetAllEnumerableKeys(vm);
  for (auto key : keys) {
    // a. Call the [[DefineOwnProperty]] internal method of array with arguments ToString(index),
    //    the PropertyDescriptor {[[Value]]: P, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    types::Object::DefineOwnProperty(vm, array, factory->NewStringFromInt(index),
                                     types::PropertyDescriptor{vm, key, true, true, true}, false);

    // b. Increment index by 1.
    ++index;
  }
  
  // 6. Return array.
  return array.GetJSValue();
}

// Object.prototype.toString()
// Defined in ECMAScript 5.1 Chapter 15.2.4.2
JSValue JSObject::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. If the this value is undefined, return "[object Undefined]".
  if (this_value->IsUndefined()) {
    return factory->NewString(u"[object Undefined]").GetJSValue();
  }
  
  // 2. If the this value is null, return "[object Null]".
  if (this_value->IsNull()) {
    return factory->NewString(u"[object Null]").GetJSValue();
  }
  
  // 3. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 4. Let class be the value of the [[Class]] internal property of O.
  ObjectClassType class_type = O->GetClassType();
  
  // 5 .Return the String value that is the result of concatenating the three Strings "[object ", class, and "]".
  return types::String::Concat(
    vm, factory->NewString(u"[object "),
    ObjectClassTypeToString(vm, class_type), factory->NewString(u"]")).GetJSValue();
}

// Object.prototype.toLocaleString()
// Defined in ECMAScript 5.1 Chapter 15.2.4.3
JSValue JSObject::ToLocaleString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let toString be the result of calling the [[Get]] internal method of O passing "toString" as the argument.
  JSHandle<JSValue> to_string = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledToStringString());
  
  // 3. If IsCallable(toString) is false, throw a TypeError exception.
  if (!to_string.As<HeapObject>()->GetCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"No toString method on current object.", JSValue{});
  }

  // 4. Return the result of calling the [[Call]] internal method of toString passing O as the this value and no arguments.
  return types::Object::Call(vm, to_string.As<Object>(), O.As<JSValue>(), {}).GetJSValue();
}

// Object.prototype.valueOf()
// Defined in ECMAScript 5.1 Chapter 15.2.4.4
JSValue JSObject::ValueOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<Object> O = JSValue::ToObject(vm, this_value);

  // 2. If O is the result of calling the Object constructor with a host object (15.2.2.1), then
  // a. Return either O or another value such as the host object originally passed to the constructor. The specific result that is returned is implementation-defined.
  // todo

  // 3. Return O.
  return O.GetJSValue();
}

// Object.hasOwnProprety(V)
// Defined in ECMAScript 5.1 Chapter 15.2.5.6
JSValue JSObject::HasOwnProperty(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> V = argv->GetArg(0);
  
  // 1. Let P be ToString(V).
  JSHandle<types::String> P = JSValue::ToString(vm, V);
  
  // 2. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  
  // 3. Let desc be the result of calling the [[GetOwnProperty]] internal method of O passing P as the argument.
  types::PropertyDescriptor desc = Object::GetOwnProperty(vm, O, P);
  
  // 4. If desc is undefined, return false.
  if (desc.IsEmpty()) {
    return JSValue::False();
  }
  
  // 5. Return true.
  return JSValue::True();
}

// Object.prototype.isPrototypeOf(V)
// Defined in ECMAScript 5.1 Chapter 15.2.4.6
JSValue JSObject::IsPrototypeOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> V = argv->GetArg(0);

  // 1. If V is not an object, return false.
  if (!V->IsObject()) {
    return JSValue::False();
  }
  
  // 2. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 3. Repeat
  while (true) {
    // a. Let V be the value of the [[Prototype]] internal property of V.
    V = JSHandle<JSValue>{vm, V.As<types::Object>()->GetPrototype()};
    
    // b. if V is null, return false
    if (V->IsNull()) {
      return JSValue::False();
    }
    
    // c. If O and V refer to the same object, return true.
    if (V.GetJSValue() == O.GetJSValue()) {
      return JSValue::True();
    }
  }
}

// Object.prototype.propertyIsEnumerable(V)
// Defined in ECMAScript 5.1 Chapter 15.2.4.7
JSValue JSObject::PropertyIsEnumerable(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> V = argv->GetArg(0);
  
  // 1. Let P be ToString(V).
  JSHandle<types::String> P = JSValue::ToString(vm, V);
  
  // 2. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 3. Let desc be the result of calling the [[GetOwnProperty]] internal method of O passing P as the argument.
  types::PropertyDescriptor desc = Object::GetOwnProperty(vm, O, P);
  
  // 4. If desc is undefined, return false.
  if (desc.IsEmpty()) {
    return JSValue::False();
  }
  
  // 5. Return the value of desc.[[Enumerable]].
  return JSValue{desc.GetEnumerable()};
}


}  // namespace builtins
}  // namespace voidjs
