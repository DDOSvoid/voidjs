#ifndef VOIDJS_TYPES_OBJECT_CLASS_TYPE_H
#define VOIDJS_TYPES_OBJECT_CLASS_TYPE_H

#include <cstdint>

namespace voidjs {

namespace types {

class String;

}  // namespace types

enum class ObjectClassType : std::uint8_t {
  GLOBAL_OBJECT,

  // Standard specified object class type
  // Defined in ECMAScript 5.1 Chapter 8.6.2
  ARGUMENTS,
  ARRAY,
  BOOLEAN,
  DATE,
  ERROR,
  FUNCTION,
  JSON,
  MATH,
  NUMBER,
  OBJECT,
  REG_EXP,
  STRING,
};

inline types::String* ObjectClassTypeToString(ObjectClassType type); 

}  // namespace voidjs 

#endif  // VOIDJS_TYPES_OBJECT_CLASS_TYPE_H
