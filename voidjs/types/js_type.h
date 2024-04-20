#ifndef VOIDJS_TYPES_JS_TYPE_H
#define VOIDJS_TYPES_JS_TYPE_H

#include <cstdint>

namespace voidjs {

enum class JSType : std::uint8_t {
  STRING,
  OBJECT,
  ARRAY,
  DATA_PROPERTY_DESCRIPTOR,
  ACCESSOR_PROPERTY_DESCRIPTOR,
  PROPERTY_MAP,
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_JS_TYPE_H
