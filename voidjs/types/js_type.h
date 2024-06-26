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
  HASH_MAP,
  ENVIRONMENT_RECORD,
  DECLARATIVE_ENVIRONMENT_RECORD,
  OBJECT_ENVIRONMENT_RECORD,
  LEXICAL_ENVIRONMENT,

  // standard builtin objects
  GLOBAL_OBJECT,
  JS_OBJECT,
  JS_FUNCTION,
  JS_ARRAY,
  JS_STRING,
  JS_BOOLEAN,
  JS_NUMBER,
  JS_MATH,
  JS_ERROR,
  ARGUMENTS,
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_JS_TYPE_H
