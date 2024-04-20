#ifndef VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/spec_types/property_descriptor.h"

namespace voidjs {
namespace types {

class PropertyMap : public Array {
 public:
  static constexpr std::size_t DEFAULT_ENTRY_NUM = 3;

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

  static JSValue SetProperty(JSValue prop_map, JSValue key, const PropertyDescriptor& desc) {
    auto arr = prop_map.GetHeapObject()->AsPropertyMap();
    
    JSValue prop;
    if (desc.IsDataDescriptor()) {
      prop = JSValue(ObjectFactory::NewDataPropertyDescriptor(desc));
    } else if (desc.IsAccessorDescriptor()) {
      prop = JSValue(ObjectFactory::NewAccessorPropertyDescriptor(desc));
    } else {
      // todo
    }
    
    auto len = arr->GetLength();
    for (std::size_t idx = 0; idx < len; idx += 2) {
      auto cur_key = arr->GetByIndex(idx);
      if (cur_key.IsEmpty()) {
        arr->SetByIndex(idx, key);
        arr->SetByIndex(idx + 1, prop);
        return prop_map;
      } 
      if (JSValue::SameValue(cur_key, key)) {
        arr->SetByIndex(idx + 1, prop);
        return prop_map;
      }
    }
    
    auto new_arr = ObjectFactory::NewArray(2);
    new_arr->SetByIndex(0, key);
    new_arr->SetByIndex(1, prop);

    return JSValue(Append(JSValue(arr), JSValue(new_arr)));
  }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

