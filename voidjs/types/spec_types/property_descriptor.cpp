#include "voidjs/types/spec_types/property_descriptor.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace types {

// FromPropertyDescriptor
// Defined in ECMAScript 5.1 Chapter 8.10.4
JSValue PropertyDescriptor::FromPropertyDescriptor(VM* vm) const {
  auto factory = vm->GetObjectFactory();
  
  // 1. If Desc is undefined, then return undefined.
    if (IsEmpty()) {
      return JSValue::Undefined();
    }
    
    // 2. Let obj be the result of creating a new object as if
    //    by the expression new Object() where Object is the standard built-in constructor with that name.
    auto obj = builtins::JSObject::Construct(
      factory->NewRuntimeCallInfo(JSValue::Undefined(), std::vector<JSValue>{}));
    
    // 3. If IsDataDescriptor(Desc) is true, then
    if (IsDataDescriptor()) {
      // a. Call the [[DefineOwnProperty]] internal method of obj with arguments "value",
      //    Property Descriptor {[[Value]]: Desc.[[Value]], [[Writable]]: true, [[Enumerable]]: true,
      //    [[Configurable]]: true}, and false.
      Object::DefineOwnProperty(vm, obj, factory->NewStringFromTable(u"value"),
                                PropertyDescriptor{GetValue(), true, true, true}, false);
      
      // b. Call the [[DefineOwnProperty]] internal method of obj with arguments "writable",
      //    Property Descriptor {[[Value]]: Desc.[[Writable]], [[Writable]]: true, [[Enumerable]]: true,
      //    [[Configurable]]: true}, and false.
      Object::DefineOwnProperty(vm, obj, factory->NewStringFromTable(u"writable"),
                                PropertyDescriptor{JSValue{GetWritable()}, true, true, true}, false);
    }
    // 4. Else, IsAccessorDescriptor(Desc) must be true, so
    else {
      // a. Call the [[DefineOwnProperty]] internal method of obj with arguments "get",
      //    Property Descriptor {[[Value]]: Desc.[[Get]], [[Writable]]: true, [[Enumerable]]: true,
      //    [[Configurable]]: true}, and false.
      Object::DefineOwnProperty(vm, obj, factory->NewStringFromTable(u"get"),
                                PropertyDescriptor{GetGetter(), true, true, true}, false);
      
      // b. Call the [[DefineOwnProperty]] internal method of obj with arguments "set",
      //    Property Descriptor {[[Value]]: Desc.[[Set]], [[Writable]]: true, [[Enumerable]]: true,
      //    [[Configurable]]: true}, and false.
      Object::DefineOwnProperty(vm, obj, factory->NewStringFromTable(u"set"),
                                PropertyDescriptor{GetSetter(), true, true, true}, false);
      
      // 5. Call the [[DefineOwnProperty]] internal method of obj with arguments "enumerable",
      //    Property Descriptor {[[Value]]: Desc.[[Enumerable]], [[Writable]]: true, [[Enumerable]]: true,
      //    [[Configurable]]: true}, and false.
      Object::DefineOwnProperty(vm, obj, factory->NewStringFromTable(u"euumerable"),
                                PropertyDescriptor{JSValue{GetEnumerable()}, true, true, true}, false);
      
      // 6. Call the [[DefineOwnProperty]] internal method of obj with arguments "configurable",
      //    Property Descriptor {[[Value]]: Desc.[[Configurable]], [[Writable]]: true, [[Enumerable]]: true,
      //    [[Configurable]]: true}, and false.
      Object::DefineOwnProperty(vm, obj, factory->NewStringFromTable(u"configurable"),
                                PropertyDescriptor{JSValue{GetConfigurable()}, true, true, true}, false);
    }
    
    // 7. Return obj.
    return JSValue{obj};
}

// ToPropertyDescriptor
// Defind in ECMAScript 5.1 Chapter 8.10.5
PropertyDescriptor PropertyDescriptor::ToPropertyDescriptor(VM* vm, JSValue obj_val) {
  auto factory = vm->GetObjectFactory();
  
  // 1. If Type(Obj) is not Object throw a TypeError exception.
  if (!obj_val.IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"ToPropertyDescriptor cannot work on non-object type.", PropertyDescriptor{});
  }
  auto obj = obj_val.GetHeapObject()->AsObject();
  
  // 2. Let desc be the result of creating a new Property Descriptor that initially has no fields.
  auto desc = PropertyDescriptor{};
  
  // 3. If the result of calling the [[HasProperty]] internal method of Obj with argument "enumerable" is true, then
  if (Object::HasProperty(vm, obj, factory->NewStringFromTable(u"enumerable"))) {
    // a. Let enum be the result of calling the [[Get]] internal method of Obj with "enumerable".
    auto enum_val = Object::Get(vm, obj, factory->NewStringFromTable(u"enumerable"));
    
    // b. Set the [[Enumerable]] field of desc to ToBoolean(enum).
    desc.SetEnumerable(JSValue::ToBoolean(vm, enum_val));
  }
  
  // 4. If the result of calling the [[HasProperty]] internal method of Obj with argument "configurable" is true, then
  if (Object::HasProperty(vm, obj, factory->NewStringFromTable(u"configurable"))) {
    // a. Let conf be the result of calling the [[Get]] internal method of Obj with argument "configurable".
    auto conf = Object::Get(vm, obj, factory->NewStringFromTable(u"configurable"));
    
    // b. Set the [[Configurable]] field of desc to ToBoolean(conf).
    desc.SetConfigurable(JSValue::ToBoolean(vm, conf));
  }
  
  // 5. If the result of calling the [[HasProperty]] internal method of Obj with argument "value" is true, then
  if (Object::HasProperty(vm, obj, factory->NewStringFromTable(u"value"))) {
    // a. Let value be the result of calling the [[Get]] internal method of Obj with argument “value”.
    auto value = Object::Get(vm, obj, factory->NewStringFromTable(u"value"));
    
    // b. Set the [[Value]] field of desc to value.
    desc.SetValue(value);
  }
  
  // 6. If the result of calling the [[HasProperty]] internal method of Obj with argument "writable" is true, then
  if (Object::HasProperty(vm, obj, factory->NewStringFromTable(u"writable"))) {
    // a. Let writable be the result of calling the [[Get]] internal method of Obj with argument "writable".
    auto writable = Object::Get(vm, obj, factory->NewStringFromTable(u"writable"));
    
    // b. Set the [[Writable]] field of desc to ToBoolean(writable).
    desc.SetWritable(JSValue::ToBoolean(vm, writable));
  }
  
  // 7. If the result of calling the [[HasProperty]] internal method of Obj with argument "get" is true, then
  if (Object::HasProperty(vm, obj, factory->NewStringFromTable(u"get"))) {
    // a. Let getter be the result of calling the [[Get]] internal method of Obj with argument "get".
    auto getter = Object::Get(vm, obj, factory->NewStringFromTable(u"get"));
    
    // b. If IsCallable(getter) is false and getter is not undefined, then throw a TypeError exception.
    if (!getter.IsCallable() && !getter.IsUndefined()) {
      THROW_TYPE_ERROR_AND_RETURN_VALUE(
        vm, u"ToPropertyDescriptor fails.", PropertyDescriptor{});
    }

    // c. Set the [[Get]] field of desc to getter.
    desc.SetGetter(getter);
  }
  // 8. If the result of calling the [[HasProperty]] internal method of Obj with argument "set" is true, then
  if (Object::HasProperty(vm, obj, factory->NewStringFromTable(u"set"))) {
    // a. Let setter be the result of calling the [[Get]] internal method of Obj with argument "set".
    auto setter = Object::Get(vm, obj, factory->NewStringFromTable(u"set"));
    
    // b. If IsCallable(setter) is false and setter is not undefined, then throw a TypeError exception.
    if (!setter.IsCallable() && !setter.IsUndefined()) {
      THROW_TYPE_ERROR_AND_RETURN_VALUE(
        vm, u"ToPropertyDescriptor fails.", PropertyDescriptor{});
    }
    
    // c. Set the [[Set]] field of desc to setter.
    desc.SetSetter(setter);
  }
  
  // 9. If either desc.[[Get]] or desc.[[Set]] are present, then
  if (desc.HasGetter() || desc.HasSetter()) {
    // a. If either desc.[[Value]] or desc.[[Writable]] are present, then throw a TypeError exception.
    if (desc.HasValue() || desc.HasWritable()) {
      THROW_TYPE_ERROR_AND_RETURN_VALUE(
        vm, u"ToPropertyDescriptor fails when the result descriptor is both a data property descriptor and an accessor property descriptor.",
        PropertyDescriptor{});
    }
  }
  
  // 10. Return desc.
  return desc;
}

}  // namespace types
}  // namespace voidjs
