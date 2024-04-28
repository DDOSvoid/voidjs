#include "voidjs/builtins/js_object.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace builtins {

// Object([value])
// Defined in ECMAScript 5.1 Chapter 15.2.1.1
// todo
JSValue JSObject::ObjectConstructor(RuntimeCallInfo* argv) {
  auto value = argv->GetArg(0);
  
  // 1. If value is null, undefined or not supplied,
  //    create and return a new Object object exactly as if
  //    the standard built-in Object constructor had been called with the same arguments (15.2.2.1).
  if (value.IsNull() || value.IsUndefined() || value.IsEmpty()) {
    return JSValue(ObjectFactory::NewJSObject(value));
  }

  // 2. Return ToObject(value).
  return JSValue(JSValue::ToObject(value));
}

// Object.getPrototypeOf(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.2
JSValue JSObject::GetPrototypeOf(RuntimeCallInfo* argv) {
  JSValue O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }
  
  // 2. Return the value of the [[Prototype]] internal property of O.
  return O.GetHeapObject()->AsObject()->GetPrototype();
}

// Object.getOwnPropertyDescriptor(O, P)
// Defined in ECMAScript 5.1 Chapter 15.2.3.3
JSValue JSObject::GetOwnPropretyDescriptor(RuntimeCallInfo* argv) {
  JSValue O = argv->GetArg(0);
  JSValue P = argv->GetArg(1);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }
  
  // 2. Let name be ToString(P).
  auto name = JSValue::ToString(P);
  
  // 3. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with argument name.
  auto desc = O.GetHeapObject()->AsObject()->GetOwnProperty(name);
  
  // 4. Return the result of calling FromPropertyDescriptor(desc) (8.10.4).
  return desc.FromPropertyDescriptor();
}

// Object.getOwnPropertyNames(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.4
// todo
JSValue JSObject::GetOwnPropertyNames(RuntimeCallInfo* argv) {
  // 1. If Type(O) is not Object throw a TypeError exception.
  // 2. Let array be the result of creating a new object as if by the expression new Array() where Array is the standard built-in constructor with that name.
  // 3. Let n be 0.
  // 4. For each named own property P of O
  // a. Let name be the String value that is the name of P.
  // b. Call the [[DefineOwnProperty]] internal method of array with arguments ToString(n), the PropertyDescriptor {[[Value]]: name, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
  // c. Increment n by 1.
  // 5. Return array.
}

// Object.create(O, [, Properties])
// Defined in ECMAScript 5.1 Chapter 15.2.3.5
JSValue JSObject::Create(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object or Null throw a TypeError exception.
  if (!O.IsObject() && !O.IsNull()) {
    // todo
  }
  
  // 2. Let obj be the result of creating a new object as if
  //    by the expression new Object() where Object is the standard built-in constructor with that name
  auto obj = ObjectFactory::NewJSObject(JSValue{});
  
  // 3. Set the [[Prototype]] internal property of obj to O.
  obj->SetPrototype(O);
  
  // 4. If the argument Properties is present and not undefined,
  //    add own properties to obj as if by calling the standard built-in function
  //    Object.defineProperties with arguments obj and Properties.
  // todo
  
  // 5. Return obj.
  return JSValue(obj);
}

// Object.defineProperty(O, P, Attributes)
// Defined in ECMAScript 5.1 Chapter 15.2.3.6
JSValue JSObject::DefineProperty(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);
  auto P = argv->GetArg(1);
  auto Attributes = argv->GetArg(2);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }
  
  // 2. Let name be ToString(P).
  auto name = JSValue::ToString(P);
  
  // 3. Let desc be the result of calling ToPropertyDescriptor with Attributes as the argument.
  // todo
  const auto& desc = types::PropertyDescriptor::ToPropertyDescriptor(Attributes);
  
  // 4. Call the [[DefineOwnProperty]] internal method of O with arguments name, desc, and true.
  O.GetHeapObject()->AsObject()->DefineOwnProperty(name, desc, true);
  
  // 5. Return O.
  return O; 
}

// Object.defineProperties(O, Properties)
// Defined in ECMAScript 5.1 Chapter 15.2.3.7
// todo
JSValue JSObject::DefineProperties(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);
  auto Properties = argv->GetArg(1);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }
  
  // 2. Let props be ToObject(Properties).
  auto props = JSValue::ToObject(Properties);
  
  // 3. Let names be an internal list containing the names of each enumerable own property of props.
  
  // 4. Let descriptors be an empty internal List.
  
  // 5. For each element P of names in list order,
  
  // a. Let descObj be the result of calling the [[Get]] internal method of props with P as the argument.
  // b. Let desc be the result of calling ToPropertyDescriptor with descObj as the argument.
  // c. Append desc to the end of descriptors.
  
  // 6. For each element desc of descriptors in list order,
  
  // 7. Call the [[DefineOwnProperty]] internal method of O with arguments P, desc, and true.
  
  // 8. Return O
  return O;
}

// Object.seal(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.8
// todo
JSValue JSObject::Seal(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);

  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }

  // 2. For each named own property name P of O,

  // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.

  // b. If desc.[[Configurable]] is true, set desc.[[Configurable]] to false.

  // c. Call the [[DefineOwnProperty]] internal method of O with P, desc, and true as arguments.

  // 3. Set the [[Extensible]] internal property of O to false.

  // 4. Return O.
  return O;
}

// Object.freeze(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.9
// todo
JSValue JSObject::Freeze(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.

  // 2. For each named own property name P of O,

  // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.

  // b. If IsDataDescriptor(desc) is true, then

  // i.If desc.[[Writable]] is true, set desc.[[Writable]] to false.

  // c. If desc.[[Configurable]] is true, set desc.[[Configurable]] to false.

  // d. Call the [[DefineOwnProperty]] internal method of O with P, desc, and true as arguments.

  // 3. Set the [[Extensible]] internal property of O to false.

  // 4. Return O.
  return O;
}

// Object.preventExtensions(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.10
JSValue JSObject::PreventExtensions(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }

  // 2. Set the [[Extensible]] internal property of O to false.
  O.GetHeapObject()->SetExtensible(false);

  // 3. Return O.
  return O;
}

// Object.isSealed(O)
// Defined in ECMAScript 5.1 Chapter 15.2.3.11
// todo
JSValue JSObject::IsSealed(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);

  // 1. If Type(O) is not Object throw a TypeError exception.

  // 2. For each named own property name P of O,

  // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.

  // b. If desc.[[Configurable]] is true, then return false.

  // 3. If the [[Extensible]] internal property of O is false, then return true.

  // 4. Otherwise, return false.
  
}

// Object.isFrozen
// Defined in ECMAScript 5.1 Chapter 15.2.3.12
// todo
JSValue JSObject::IsFrozen(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);

  // 1. If Type(O) is not Object throw a TypeError exception.

  // 2. For each named own property name P of O,

  // a. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with P.

  // b. If IsDataDescriptor(desc) is true then

  // i. If desc.[[Writable]] is true, return false.

  // c. If desc.[[Configurable]] is true, then return false.

  // 3. If the [[Extensible]] internal property of O is false, then return true.

  // 4. Otherwise, return false.
}

// Object.isExtensible
// Defined in ECMAScript 5.1 Chatper 15.2.3.13
JSValue JSObject::IsExtensible(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);
  
  // 1. If Type(O) is not Object throw a TypeError exception.
  if (!O.IsObject()) {
    // todo
  }

  // 2. Return the Boolean value of the [[Extensible]] internal property of O.
  return JSValue(O.GetHeapObject()->GetExtensible());
}

// Object.keys
// Defined in ECMAScript 5.1 Chapter 15.2.3.14
// todo
JSValue JSObject::Keys(RuntimeCallInfo* argv) {
  auto O = argv->GetArg(0);

  // 1. If the Type(O) is not Object, throw a TypeError exception.

  // 2. Let n be the number of own enumerable properties of O

  // 3. Let array be the result of creating a new Object as if by the expression new Array(n) where Array is the standard built-in constructor with that name.

  // 4. Let index be 0.

  // 5. For each own enumerable property of O whose name String is P

  // a. Call the [[DefineOwnProperty]] internal method of array with arguments ToString(index), the PropertyDescriptor {[[Value]]: P, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.

  // b. Increment index by 1.

  // 6. Return array.
}

// Object.prototype.toString()
// Defined in ECMAScript 5.1 Chapter 15.2.4.2
JSValue JSObject::ToString(RuntimeCallInfo* argv) {
  // 1. If the this value is undefined, return "[object Undefined]".
  // 2. If the this value is null, return "[object Null]".
  // 3. Let O be the result of calling ToObject passing the this value as the argument.
  // 4. Let class be the value of the [[Class]] internal property of O.
  // 5 .Return the String value that is the result of concatenating the three Strings "[object ", class, and "]".
}

// Object.prototype.toLocaleString()
// Defined in ECMAScript 5.1 Chapter 15.2.4.3
// todo
JSValue JSObject::ToLocalString(RuntimeCallInfo* argv) {
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  // 2. Let toString be the result of calling the [[Get]] internal method of O passing "toString" as the argument.
  // 3. If IsCallable(toString) is false, throw a TypeError exception.
  // 4. Return the result of calling the [[Call]] internal method of toString passing O as the this value and no arguments.
}

// Object.prototype.valueOf()
// Defined in ECMAScript 5.1 Chapter 15.2.4.4
JSValue JSObject::ValueOf(RuntimeCallInfo* argv) {
  auto this_obj = argv->GetThis();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  auto O = JSValue::ToObject(this_obj);

  // 2. If O is the result of calling the Object constructor with a host object (15.2.2.1), then
  // a. Return either O or another value such as the host object originally passed to the constructor. The specific result that is returned is implementation-defined.
  // todo

  // 3. Return O.
  return JSValue(O);
}

// Object.hasOwnProprety(V)
// Defined in ECMAScript 5.1 Chapter 15.2.5.6
JSValue JSObject::HasOwnProperty(RuntimeCallInfo* argv) {
  auto this_obj = argv->GetThis();
  auto V = argv->GetArg(0);
  
  // 1. Let P be ToString(V).
  auto P = JSValue::ToString(V);
  
  // 2. Let O be the result of calling ToObject passing the this value as the argument.
  auto O = JSValue::ToString(this_obj);
  
  // 3. Let desc be the result of calling the [[GetOwnProperty]] internal method of O passing P as the argument.
  auto desc = O.GetHeapObject()->AsObject()->GetOwnProperty(P);
  
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
  auto this_obj = argv->GetThis();
  auto V = argv->GetArg(0);

  // 1. If V is not an object, return false.
  if (!V.IsObject()) {
    return JSValue::False();
  }
  
  // 2. Let O be the result of calling ToObject passing the this value as the argument.
  auto O = JSValue::ToObject(this_obj);
  
  // 3. Repeat
  while (true) {
    // a. Let V be the value of the [[Prototype]] internal property of V.
    auto proto = V.GetHeapObject()->AsObject()->GetPrototype();
    
    // b. if V is null, return false
    if (proto.IsNull()) {
      return JSValue::False();
    }
    // c. If O and V refer to the same object, return true.
    if (proto.GetRawData() == reinterpret_cast<JSValueType>(O)) {
      return JSValue::True();
    }
  }
}

// Object.prototype.propertyIsEnumerable(V)
// Defined in ECMAScript 5.1 Chapter 15.2.4.7
JSValue JSObject::PropertyIsEnumerable(RuntimeCallInfo* argv) {
  auto this_obj = argv->GetThis();
  auto V = argv->GetArg(0);
  
  // 1. Let P be ToString(V).
  auto P = JSValue::ToString(V);
  
  // 2. Let O be the result of calling ToObject passing the this value as the argument.
  auto O = JSValue::ToObject(this_obj);
  
  // 3. Let desc be the result of calling the [[GetOwnProperty]] internal method of O passing P as the argument.
  auto desc = O->GetOwnProperty(P);
  
  // 4. If desc is undefined, return false.
  if (desc.IsEmpty()) {
    return JSValue::False();
  }
  
  // 5. Return the value of desc.[[Enumerable]].
  return JSValue(desc.GetEnumerable());
}


}  // namespace builtins
}  // namespace voidjs
