#ifndef VOIDJS_TYPES_ERROR_TYPE_H
#define VOIDJS_TYPES_ERROR_TYPE_H

#include <cstdint>

namespace voidjs {

enum class ErrorType : std::uint8_t {
  ERROR,
  EVAL_ERROR,
  RANGE_ERROR,
  REFERENCE_ERROR,
  SYNTAX_ERROR,
  TYPE_ERROR,
  URI_ERROR,
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_ERROR_TYPE_H
