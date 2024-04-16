#ifndef VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
#define VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class Reference {
 public:
  JSValue GetBase() const { return base_; }

  bool HasPrimitiveBase() const { return base_.IsBoolean() || base_.IsString() || base_.IsNumber(); }
  
  bool IsUnresolvableReference() const { return base_.IsUndefined(); }
  

 private:
  JSValue base_;
  std::u16string name_;
  bool is_strict_;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
