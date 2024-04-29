#ifndef VOIDJS_TYPES_JS_TYPE_H
#define VOIDJS_TYPES_JS_TYPE_H

#include <cstdint>

namespace voidjs {

enum class JSType : std::uint8_t {
  // language types
  STRING,
  OBJECT,

  // internal types
  ARRAY,
  DATA_PROPERTY_DESCRIPTOR,
  ACCESSOR_PROPERTY_DESCRIPTOR,
  GENERIC_PROPERTY_DESCRIPTOR,
  PROPERTY_MAP,
  BINDING,
  INTERNAL_FUNCTION,

  // standard builtin objects
  GLOBAL_OBJECT,
  JS_OBJECT,
  JS_FUNCTION,
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_JS_TYPE_H
