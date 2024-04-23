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
  Reference(const std::variant<JSValue, EnvironmentRecord*>& base,
            String* name, bool is_strict)
    : base_(base), name_(name), is_strict_(is_strict)
  {}
  
  // GetBase
  // Returns the base value component of the reference V.
  const std::variant<JSValue, EnvironmentRecord*>& GetBase() const { return base_; }

  // GetReferencedName
  // Returns the referenced name component of the reference V.
  String* GetReferencedName() const { return name_; }

  // IsStrictReference
  // Returns the strict reference component of the reference V.
  bool IsStrictReference() const { return is_strict_; }

  // HasPrimitiveBase
  // Returns true if the base value is a Boolean, String, or Number.
  bool HasPrimitiveBase() const {
    if (auto val = std::get_if<JSValue>(&base_); !val) {
      return false;
    } else {
      return val->IsBoolean() || val->IsString() || val->IsNumber();
    }
  }

  // IsPropertyReference
  // Returns true if either the base value is an object
  // or HasPrimitiveBase(V) is true; otherwise returns false.
  bool IsPropertyReference() const {
    auto val = std::get_if<JSValue>(&base_);
    return (val && val->IsObject()) || HasPrimitiveBase();
  }

  // IsUnresolvableReference
  // Returns true if the base value is undefined and false otherwise.
  bool IsUnresolvableReference() const {
    auto val = std::get_if<JSValue>(&base_);
    return val && val->IsUndefined();
  }

 private:
  std::variant<JSValue, EnvironmentRecord*> base_;
  String* name_;
  bool is_strict_ {};
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_REFERENCE_H
