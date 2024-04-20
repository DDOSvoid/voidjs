#ifndef VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

#include "voidjs/types/heap_object.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/property_descriptor.h"

namespace voidjs {
namespace types {

class PropertyMap : public Array {
 public:
  JSValue GetProperty(JSValue key) {
    // assert(key.IsPropertyKey());

    auto len = GetLength();
    for (std::size_t idx = 0; idx < len; idx += 2) {
      auto cur_key = GetByIndex(idx);
      if (JSValue::SameValue(cur_key, key)) {
        return GetByIndex(idx + 1);
      }
    }

    return JSValue{};
  }

  void SetProperty(JSValue key, const PropertyDescriptor& desc) {
    auto len = GetLength();

    JSValue prop;
    if (desc.IsDataDescriptor()) {
      prop = JSValue(DataPropertyDescriptor::New(desc));
    } else if (desc.IsAccessorDescriptor()) {
      prop = JSValue(AccessorPropertyDescriptor::New(desc));
    } else {
      // todo
    }
    
    for (std::size_t idx = 0; idx < len; idx += 2) {
      auto cur_key = GetByIndex(idx);
      if (JSValue::SameValue(cur_key, key)) {
        SetByIndex(idx + 1, prop);
        return ;
      }
    }
    
    auto arr = Array::New(2);
    SetByIndex(0, key);
    SetByIndex(1, prop);
  }
 
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

