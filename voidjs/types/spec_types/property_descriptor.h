#ifndef VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
#define VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H

#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class PropertyDescriptor {
 public:
  PropertyDescriptor() {}
  
  explicit PropertyDescriptor(JSValue value)
    : value_(value)
  {}

  PropertyDescriptor(JSValue value, bool w, bool e, bool c)
    : value_(value), writable_(w), enumerable_(e), configurable_(c)
  {}

  JSValue GetValue() const { return value_; }
  void SetValue(JSValue value) { value_ = value; }

  bool GetWritable() const { return writable_; }
  void SetWritable(bool flag) { writable_ = flag; }

  bool GetEnumerable() const { return enumerable_; }
  void SetEnumerable(bool flag) { enumerable_ = flag; }

  bool GetConfigurable() const { return configurable_; }
  void SetConfigurable(bool flag) { configurable_ = flag; }

  JSValue GetGetter() const { return getter_; }
  void SetGetter(JSValue value) { getter_ = value; }

  JSValue GetSetter() const { return setter_; }
  void SetSetter(JSValue value) { setter_ = value; }

 private:
  bool writable_ {false};
  bool enumerable_ {false};
  bool configurable_ {false};

  JSValue value_;
  JSValue getter_;
  JSValue setter_;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_PROPERTY_DESCRIPTOR_H
