#ifndef VOIDJS_TYPES_INTERNAL_TYPES_INTERNAL_FUNCTION_H
#define VOIDJS_TYPES_INTERNAL_TYPES_INTERNAL_FUNCTION_H

#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace types {

class InternalFunction : public types::Object {
 public:
  static constexpr std::size_t FUNCTION_OFFSET = Object::END_OFFSET;
  InternalFunctionType GetFunction() const {
    return *utils::BitGet<InternalFunctionType*>(this, FUNCTION_OFFSET);
  }
  void SetFunction(InternalFunctionType func) {
    *utils::BitGet<InternalFunctionType*>(this, FUNCTION_OFFSET) = func;
  }

  static constexpr std::size_t SIZE = sizeof(InternalFunctionType);
  static constexpr std::size_t END_OFFSET = Object::END_OFFSET + SIZE;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPES_INTERNAL_FUNCTION_H
