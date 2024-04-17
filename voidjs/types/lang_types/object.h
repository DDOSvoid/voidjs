#ifndef VOIDJS_TYPES_LANG_TYPES_OBJECT_H
#define VOIDJS_TYPES_LANG_TYPES_OBJECT_H

#include <cstddef>
#include <cstdint>

#include "voidjs/utils/helper.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"

namespace voidjs {
namespace types {

class String;

class Object {
 public:
  // JSType type_;
  static constexpr std::size_t TYPE_OFFSET = 0;
  JSType GetType() const { return *utils::BitGet<JSType*>(this, TYPE_OFFSET); };
  void SetType(JSType type) { *utils::BitGet<JSType*>(this, TYPE_OFFSET) = type; }

  static constexpr std::size_t SIZE = TYPE_OFFSET + sizeof(JSType);

  static Object* New(std::size_t size, JSType type) {
    auto obj = reinterpret_cast<Object*>(Allocate(size));
    obj->SetType(type);
    return obj;
  }
  
  static std::uintptr_t Allocate(std::size_t size) {
    return reinterpret_cast<std::uintptr_t>(new std::byte[SIZE + size]);
  }
  
  // Is Check
  bool IsString() const { return GetType() == JSType::STRING; }

  // As Cast
  String* AsString() { return reinterpret_cast<String*>(this); }

  // As Cast
  const String* AsString() const { return reinterpret_cast<const String*>(this); }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANG_TYPES_OBJECT_H
