#ifndef VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/hash_map.h"

namespace voidjs {
namespace types {

class PropertyMap : public HashMap {
 public:
  static constexpr std::uint32_t DEFAULT_PROPERTY_NUMS = 4;
  
  JSValue GetProperty(VM* vm, String* key) {
    return Find(vm, key);
  }

  static PropertyMap* SetProperty(VM* vm, PropertyMap* prop_map, String* key, const PropertyDescriptor& desc) {
    auto factory = vm->GetObjectFactory();
    
    JSValue prop;
    if (desc.IsDataDescriptor()) {
      prop = JSValue(factory->NewDataPropertyDescriptor(desc));
    } else if (desc.IsAccessorDescriptor()) {
      prop = JSValue(factory->NewAccessorPropertyDescriptor(desc));
    } else {
      prop = JSValue(factory->NewGenericPropertyDescriptor(desc));
    }

    return Insert(vm, prop_map, key, prop)->AsPropertyMap();
  }

  void DeleteProperty(VM* vm, String* key) {
    Erase(vm, key);
  }

};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

