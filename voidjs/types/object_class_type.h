#ifndef VOIDJS_TYPES_OBJECT_CLASS_TYPE_H
#define VOIDJS_TYPES_OBJECT_CLASS_TYPE_H

#include <cstdint>

namespace voidjs {

class VM;
template <typename T>
class JSHandle;

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

inline JSHandle<types::String> ObjectClassTypeToString(VM* vm, ObjectClassType type); 

}  // namespace voidjs 

#endif  // VOIDJS_TYPES_OBJECT_CLASS_TYPE_H
