#ifndef VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
#define VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H

#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/gc/js_handle.h"
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
  PropertyDescriptor() = default;
  
  explicit PropertyDescriptor(VM* vm)
    : vm_(vm)
  {}
  
  PropertyDescriptor(VM* vm, JSHandle<JSValue> value)
    : vm_(vm), value_(value)
  {}

  PropertyDescriptor(VM* vm, JSHandle<JSValue> value, bool w, bool e, bool c)
    : vm_(vm), value_(value), writable_(w), enumerable_(e), configurable_(c),
      has_writable_(true), has_enumerable_(true), has_configurable_(true)
  {}

  PropertyDescriptor(VM* vm, JSHandle<JSValue> getter, JSHandle<JSValue> setter, bool e, bool c)
    : vm_(vm), getter_(getter), setter_(setter), enumerable_(e), configurable_(c),
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
  JSHandle<JSValue> GetValue() const { return HasValue() ? value_ : JSHandle<JSValue>{vm_, JSValue::Undefined()}; }
  void SetValue(JSHandle<JSValue> value) { value_ = value; }
  void SetValue(JSValue value) { value_ = JSHandle<JSValue>{vm_, value}; }

  bool HasGetter() const { return !getter_.IsEmpty(); }
  JSHandle<JSValue> GetGetter() const { return HasGetter() ? getter_ : JSHandle<JSValue>{vm_, JSValue::Undefined()}; }
  void SetGetter(JSHandle<JSValue> value) { getter_ = value; }
  void SetGetter(JSValue value) { getter_ = JSHandle<JSValue>{vm_, value}; }

  bool HasSetter() const { return !setter_.IsEmpty(); }
  JSHandle<JSValue> GetSetter() const { return HasSetter() ? setter_ : JSHandle<JSValue>{vm_, JSValue::Undefined()}; }
  void SetSetter(JSHandle<JSValue> value) { setter_ = value; }
  void SetSetter(JSValue value) { setter_ = JSHandle<JSValue>{vm_, value}; }

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
  JSHandle<JSValue> FromPropertyDescriptor() const;

  // ToPropertyDescriptor
  // Defind in ECMAScript 5.1 Chapter 8.10.5
  static PropertyDescriptor ToPropertyDescriptor(VM* vm, JSHandle<JSValue> obj);
  
  bool IsEmpty() const {
    return
      !has_writable_ && !has_enumerable_ && !has_configurable_ &&
      !HasValue() && !HasGetter() && !HasSetter();
  };

 private:
  VM* vm_                {nullptr};
  
  bool writable_         {false};
  bool enumerable_       {false};
  bool configurable_     {false};
  bool has_writable_     {false};
  bool has_enumerable_   {false};
  bool has_configurable_ {false};

  JSHandle<JSValue> value_;
  JSHandle<JSValue> getter_;
  JSHandle<JSValue> setter_;
};

class DataPropertyDescriptor : public HeapObject {
 public:
  static constexpr std::size_t VALUE_OFFSET = HeapObject::END_OFFSET;
  JSValue GetValue() { return *utils::BitGet<JSValue*>(this, VALUE_OFFSET); }
  void SetValue(JSValue value) { *utils::BitGet<JSValue*>(this, VALUE_OFFSET) = value; }
  void SetValue(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, VALUE_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

class AccessorPropertyDescriptor : public HeapObject {
 public:
  static constexpr std::size_t GETTER_OFFSET = HeapObject::END_OFFSET;
  JSValue GetGetter() { return *utils::BitGet<JSValue*>(this, GETTER_OFFSET); }
  void SetGetter(JSValue value) { *utils::BitGet<JSValue*>(this, GETTER_OFFSET) = value; }
  void SetGetter(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, GETTER_OFFSET) = handle.GetJSValue(); }
  
  static constexpr std::size_t SETTER_OFFSET = GETTER_OFFSET + sizeof(JSValue);
  JSValue GetSetter() { return *utils::BitGet<JSValue*>(this, SETTER_OFFSET); }
  void SetSetter(JSValue value) { *utils::BitGet<JSValue*>(this, SETTER_OFFSET) = value; }
  void SetSetter(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, SETTER_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t SIZE = sizeof(JSValue) + sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

class GenericPropertyDescriptor : public HeapObject {
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
