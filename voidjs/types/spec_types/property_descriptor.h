#ifndef VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
#define VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H

#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace types {

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
  JSValue GetValue() const { return value_; }
  void SetValue(JSValue value) { value_ = value; }

  bool HasGetter() const { return !getter_.IsEmpty(); }
  JSValue GetGetter() const { return getter_; }
  void SetGetter(JSValue value) { getter_ = value; }

  bool HasSetter() const { return !setter_.IsEmpty(); }
  JSValue GetSetter() const { return setter_; }
  void SetSetter(JSValue value) { setter_ = value; }

  // Defined in ECMAScript 5.1 Chatper 8.10.1
  bool IsAccessorDescriptor() const {
    // 1. If Desc is undefined, then return false.
    // 2. If both Desc.[[Get]] and Desc.[[Set]] are absent, then return false.
    // 3. Return true.
    return HasGetter() || HasSetter();
  }
  
  // Defined in ECMAScript 5.1 Chatper 8.10.2
  bool IsDataDescriptor() const {
    // 1. If Desc is undefined, then return false.
    // 2. If both Desc.[[Value]] and Desc.[[Writable]] are absent, then return false.
    // 3. Return true.
    return HasValue() || HasWritable();
  }

  bool IsGenericDescriptor() const {
    // 1. If Desc is undefined, then return false.
    // 2. If IsAccessorDescriptor(Desc) and IsDataDescriptor(Desc) are both false, then return true.
    // 3. Return false.
    return !IsAccessorDescriptor() && !IsDataDescriptor();
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
  static constexpr std::size_t VALUE_OFFSET = HeapObject::SIZE;
  JSValue GetValue() { return *utils::BitGet<JSValue*>(this, VALUE_OFFSET); }
  void SetValue(JSValue value) { *utils::BitGet<JSValue*>(this, VALUE_OFFSET) = value; }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t OFFSET = VALUE_OFFSET + sizeof(JSValue);
};

class AccessorPropertyDescriptor : public HeapObject {
 public:
  static constexpr std::size_t GETTER_OFFSET = HeapObject::OFFSET;
  JSValue GetGetter() { return *utils::BitGet<JSValue*>(this, GETTER_OFFSET); }
  void SetGetter(JSValue value) { *utils::BitGet<JSValue*>(this, GETTER_OFFSET) = value; }
  
  static constexpr std::size_t SETTER_OFFSET = GETTER_OFFSET + sizeof(JSValue);
  JSValue SetGetter() { return *utils::BitGet<JSValue*>(this, SETTER_OFFSET); }
  void SetSetter(JSValue value) { *utils::BitGet<JSValue*>(this, SETTER_OFFSET) = value; }

  static constexpr std::size_t SIZE = sizeof(JSValue) + sizeof(JSValue);
  static constexpr std::size_t OFFSET = SETTER_OFFSET + sizeof(JSValue);
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
