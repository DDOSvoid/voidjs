#ifndef VOIDJS_TYPES_SPEC_TYPES_COMPLETION_H
#define VOIDJS_TYPES_SPEC_TYPES_COMPLETION_H

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

enum class CompletionType {
  NORMAL,
  BREAK,
  CONTINUE,
  RETURN,
  THROW,
};

class Completion {
 public:
  Completion() {}
  
  explicit Completion(CompletionType type)
    : type_(type)
  {}

  Completion(CompletionType type, JSValue value)
    : type_(type), value_(value)
  {}
  
  Completion(CompletionType type, JSValue value, JSValue target)
    : type_(type), value_(value), target_(target)
  {}

  CompletionType GetType() const { return type_; }
  JSValue GetValue() const { return value_; }
  JSValue GetTarget() const { return target_; }

  bool IsAbruptCompletion() const { return type_ != CompletionType::NORMAL; }
  
 private:
  CompletionType type_ {CompletionType::NORMAL};
  JSValue value_;
  JSValue target_; 
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_COMPLETION_H
