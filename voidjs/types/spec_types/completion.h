#ifndef VOIDJS_TYPES_SPEC_TYPES_COMPLETION_H
#define VOIDJS_TYPES_SPEC_TYPES_COMPLETION_H

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

enum CompletionType {
  NORMAL,
  BREAK,
  CONTINUE,
  RETURN,
  THROW,
};

class Completion {
 public:
  
 private:
  CompletionType type_;
  JSValue value_;
  std::u16string target_;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_COMPLETION_H
