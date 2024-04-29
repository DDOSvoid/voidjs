#ifndef VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
#define VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H

#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace types {

// A data property descriptor is one that includes any fields named either [[Value]] or [[Writable]].
// An accessor property descriptor is one that includes any fields named either [[Get]] or [[Set]].
// Any property descriptor may have fields named [[Enumerable]] and [[Configurable]].
// A Property Descriptor value may not be both a data property descriptor and an accessor property descriptor.
// A generic property descriptor is a Property Descriptor value
// that is neither a data property descriptor nor an accessor property descriptor.
class PropertyDescriptor {
 public:
  PropertyDescriptor() {}
  
  explicit PropertyDescriptor(JSValue value)
    : value_(value)
  {}

  PropertyDescriptor(JSValue value, bool w, bool e, bool c)
    : value_(value), writable_(w), enumerable_(e), configurable_(c),
      has_writable_(true), has_enumerable_(true), has_configurable_(true)
  {}

  PropertyDescriptor(JSValue getter, JSValue setter, bool e, bool c)
    : getter_(getter), setter_(setter), enumerable_(e), configurable_(c),
      has_enumerable_(true), has_configurable_(true)
  {}

  bool HasWritable() const { return has_writable_; }
  bool GetWritable() const { return writable_; }
  void SetWritable(bool flag) {
    writable_ = flag;
    has_writable_ = true;
  }

  bool HasEnumerable() const { return has_enumerable_; }
  bool GetEnumerable() const { return enumerable_; }
  void SetEnumerable(bool flag) {
    enumerable_ = flag;
    has_enumerable_ = true;
  }

  bool HasConfigurable() const { return has_configurable_; }
  bool GetConfigurable() const { return configurable_; }
  void SetConfigurable(bool flag) {
    configurable_ = flag;
    has_configurable_ = true;
  }
  
  bool HasValue() const { return !value_.IsEmpty(); }
  JSValue GetValue() const { return value_.IsEmpty() ? JSValue::Undefined() : value_; }
  void SetValue(JSValue value) { value_ = value; }

  bool HasGetter() const { return !getter_.IsEmpty(); }
  JSValue GetGetter() const { return getter_.IsEmpty() ? JSValue::Undefined() : getter_; }
  void SetGetter(JSValue value) { getter_ = value; }

  bool HasSetter() const { return !setter_.IsEmpty(); }
  JSValue GetSetter() const { return setter_.IsEmpty() ? JSValue::Undefined() : setter_; }
  void SetSetter(JSValue value) { setter_ = value; }

  // IsAccessorDescriptor
  // Defined in ECMAScript 5.1 Chatper 8.10.1
  bool IsAccessorDescriptor() const {
    // 1. If Desc is undefined, then return false.
    // 2. If both Desc.[[Get]] and Desc.[[Set]] are absent, then return false.
    // 3. Return true.
    return HasGetter() || HasSetter();
  }
  
  // IsDataDescriptor
  // Defined in ECMAScript 5.1 Chatper 8.10.2
  bool IsDataDescriptor() const {
    // 1. If Desc is undefined, then return false.
    // 2. If both Desc.[[Value]] and Desc.[[Writable]] are absent, then return false.
    // 3. Return true.
    return HasValue() || HasWritable();
  }

  // IsGenericDescriptor
  // Defined in ECMAScript 5.1 Chapter 8.10.3
  bool IsGenericDescriptor() const {
    // 1. If Desc is undefined, then return false.
    // 2. If IsAccessorDescriptor(Desc) and IsDataDescriptor(Desc) are both false, then return true.
    // 3. Return false.
    return !IsAccessorDescriptor() && !IsDataDescriptor();
  }

  // FromPropertyDescriptor
  // Defined in ECMAScript 5.1 Chapter 8.10.4
  JSValue FromPropertyDescriptor() const {
    // 1. If Desc is undefined, then return undefined.
    // 2. Let obj be the result of creating a new object as if by the expression new Object() where Object is the standard built-in constructor with that name.
    // 3. If IsDataDescriptor(Desc) is true, then
    // a. Call the [[DefineOwnProperty]] internal method of obj with arguments "value", Property Descriptor {[[Value]]: Desc.[[Value]], [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    // b. Call the [[DefineOwnProperty]] internal method of obj with arguments "writable", Property Descriptor {[[Value]]: Desc.[[Writable]], [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    // 4. Else, IsAccessorDescriptor(Desc) must be true, so
    // a. Call the [[DefineOwnProperty]] internal method of obj with arguments "get", Property Descriptor {[[Value]]: Desc.[[Get]], [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    // b. Call the [[DefineOwnProperty]] internal method of obj with arguments "set", Property Descriptor {[[Value]]: Desc.[[Set]], [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    // 5. Call the [[DefineOwnProperty]] internal method of obj with arguments "enumerable", Property Descriptor {[[Value]]: Desc.[[Enumerable]], [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    // 6. Call the [[DefineOwnProperty]] internal method of obj with arguments "configurable", Property Descriptor {[[Value]]: Desc.[[Configurable]], [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
    // 7. Return obj.
  }

  // ToPropertyDescriptor
  // Defind in ECMAScript 5.1 Chapter 8.10.5
  static PropertyDescriptor ToPropertyDescriptor(JSValue ) {
    
  }
  
  bool IsEmpty() const {
    return
      !has_writable_ && !has_enumerable_ && !has_configurable_ &&
      !HasValue() && !HasGetter() && !HasSetter();
  };

 private:
  bool writable_         {false};
  bool enumerable_       {false};
  bool configurable_     {false};
  bool has_writable_     {false};
  bool has_enumerable_   {false};
  bool has_configurable_ {false};

  JSValue value_;
  JSValue getter_;
  JSValue setter_;
};

class DataPropertyDescriptor : public HeapObject {
 public:
  static constexpr std::size_t VALUE_OFFSET = HeapObject::END_OFFSET;
  JSValue GetValue() { return *utils::BitGet<JSValue*>(this, VALUE_OFFSET); }
  void SetValue(JSValue value) { *utils::BitGet<JSValue*>(this, VALUE_OFFSET) = value; }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

class AccessorPropertyDescriptor : public HeapObject {
 public:
  static constexpr std::size_t GETTER_OFFSET = HeapObject::END_OFFSET;
  JSValue GetGetter() { return *utils::BitGet<JSValue*>(this, GETTER_OFFSET); }
  void SetGetter(JSValue value) { *utils::BitGet<JSValue*>(this, GETTER_OFFSET) = value; }
  
  static constexpr std::size_t SETTER_OFFSET = GETTER_OFFSET + sizeof(JSValue);
  JSValue GetSetter() { return *utils::BitGet<JSValue*>(this, SETTER_OFFSET); }
  void SetSetter(JSValue value) { *utils::BitGet<JSValue*>(this, SETTER_OFFSET) = value; }

  static constexpr std::size_t SIZE = sizeof(JSValue) + sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

class GenericPropertyDescriptor : public HeapObject {
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
