#ifndef VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
#define VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H

#include <string_view>

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class Reference {
 public:
  // GetBase(V). Returns the base value component of the reference V.
  JSValue GetBase() const { return base_; }

  // GetReferencedName(V). Returns the referenced name component of the reference V.
  std::u16string_view GetReferencedName() const { return name_; }

  // IsStrictReference(V). Returns the strict reference component of the reference V.
  bool IsStrictReference() const { return is_strict_; }

  // HasPrimitiveBase(V). Returns true if the base value is a Boolean, String, or Number.
  bool HasPrimitiveBase() const { return base_.IsBoolean() || base_.IsString() || base_.IsNumber(); }

  // IsPropertyReference(V). Returns true if either the base value is an object
  // or HasPrimitiveBase(V) is true; otherwise returns false.
  bool IsPropertyReference() const { return base_.IsObject() || HasPrimitiveBase(); }

  // IsUnresolvableReference(V). Returns true if the base value is undefined and false otherwise.
  bool IsUnresolvableReference() const { return base_.IsUndefined(); }

 private:
  JSValue base_;
  std::u16string_view name_;
  bool is_strict_;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
