#ifndef VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/hash_map.h"
#include "voidjs/gc/js_handle.h"

namespace voidjs {
namespace types {

class PropertyMap : public HashMap {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = HashMap::END_OFFSET + SIZE;
  
  static constexpr std::uint32_t DEFAULT_PROPERTY_NUMS = 4;
  
  JSHandle<JSValue> GetProperty(VM* vm, JSHandle<String> key) {
    return Find(vm, key);
  }

  static JSHandle<PropertyMap> SetProperty(VM* vm, JSHandle<PropertyMap> prop_map, JSHandle<String> key, const PropertyDescriptor& desc) {
    auto factory = vm->GetObjectFactory();
    
    JSHandle<JSValue> prop;
    if (desc.IsDataDescriptor()) {
      prop = factory->NewDataPropertyDescriptor(desc).As<JSValue>();
    } else if (desc.IsAccessorDescriptor()) {
      prop = factory->NewAccessorPropertyDescriptor(desc).As<JSValue>();
    } else {
      prop = factory->NewGenericPropertyDescriptor(desc).As<JSValue>();
    }

    return Insert(vm, prop_map, key, prop).As<PropertyMap>();
  }

  void DeleteProperty(VM* vm, JSHandle<String> key) {
    Erase(vm, key);
  }

};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

