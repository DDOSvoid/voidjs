#ifndef VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
#define VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H

#include <string_view>
#include <variant>

#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/environment_record.h"

namespace voidjs {
namespace types {

class Reference {
 public:
  Reference(JSHandle<JSValue>base, JSHandle<String> name, bool is_strict)
    : base_(base), name_(name), is_strict_(is_strict)
  {}
  
  // GetBase
  // Returns the base value component of the reference V.
  JSHandle<JSValue> GetBase() const { return base_; }

  // GetReferencedName
  // Returns the referenced name component of the reference V.
  JSHandle<String> GetReferencedName() const { return name_; }

  // IsStrictReference
  // Returns the strict reference component of the reference V.
  bool IsStrictReference() const { return is_strict_; }

  // HasPrimitiveBase
  // Returns true if the base value is a Boolean, String, or Number.
  bool HasPrimitiveBase() const {
    return base_->IsBoolean() || base_->IsString() || base_->IsNumber();
  }

  // IsPropertyReference
  // Returns true if either the base value is an object
  // or HasPrimitiveBase(V) is true; otherwise returns false.
  bool IsPropertyReference() const {
    return base_->IsObject() && !base_->GetHeapObject()->IsEnvironmentRecord() || HasPrimitiveBase();
  }

  // IsUnresolvableReference
  // Returns true if the base value is undefined and false otherwise.
  bool IsUnresolvableReference() const {
    return base_->IsUndefined();
  }

 private:
  JSHandle<JSValue> base_;
  JSHandle<String> name_;
  bool is_strict_ {};
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
