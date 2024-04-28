#ifndef VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"

namespace voidjs {
namespace types {

class PropertyMap : public Array {
 public:
  static constexpr std::size_t DEFAULT_ENTRY_NUM = 3;

  JSValue GetProperty(JSValue key) {
    auto len = GetLength();
    for (std::size_t idx = 0; idx < len; idx += 2) {
      auto cur_key = Get(idx);
      if (JSValue::SameValue(cur_key, key)) {
        return Get(idx + 1);
      }
    }

    return JSValue{};
  }

  static PropertyMap* SetProperty(VM* vm, PropertyMap* prop_map, JSValue key, const PropertyDescriptor& desc) {
    auto factory = vm->GetObjectFactory();
    
    JSValue prop;
    if (desc.IsDataDescriptor()) {
      prop = JSValue(factory->NewDataPropertyDescriptor(desc));
    } else if (desc.IsAccessorDescriptor()) {
      prop = JSValue(factory->NewAccessorPropertyDescriptor(desc));
    } else {
      prop = JSValue(factory->NewGenericPropertyDescriptor(desc));
    }
    
    auto len = prop_map->GetLength();
    for (std::size_t idx = 0; idx < len; idx += 2) {
      auto cur_key = prop_map->Get(idx);
      if (cur_key.IsEmpty() || JSValue::SameValue(cur_key, key)) {
        if (cur_key.IsEmpty()) {
          prop_map->Set(idx, key);
        }
        prop_map->Set(idx + 1, prop);
        return prop_map;
      }
    }
    
    auto new_prop_map = factory->NewPropertyMap();
    new_prop_map->Set(0, key);
    new_prop_map->Set(1, prop);

    return Append(vm, prop_map, new_prop_map)->AsPropertyMap();
  }

  void DeleteProperty(JSValue key) {
    auto len = GetLength();
    for (std::size_t idx = 0; idx < len; idx += 2) {
      auto cur_key = Get(idx);
      if (JSValue::SameValue(cur_key, key)) {
        Set(idx, JSValue{});
        return ;
      }
    }
  }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

