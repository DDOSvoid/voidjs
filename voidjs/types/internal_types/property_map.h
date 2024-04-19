#ifndef VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

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
      if (cur_key == key) {
        return cur_key;
      }
    }

    return JSValue{};
  }

  void SetProperty(JSValue key, const PropertyDescriptor& desc) {
    
  }
 
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_PROPERTY_MAP_H

