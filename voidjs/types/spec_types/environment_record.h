#ifndef VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
#define VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H

#include <string_view>

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class EnvironmentRecord {
 public:

  // Abstract Methods of Environment Records
  // Defined in ECMAScript 5.1 Chapter 10.2.1
  virtual bool HasBinding(std::u16string_view N) const;
  virtual void CreateMutableBinding(std::u16string_view N, bool D);
  virtual void SetMutableBinding(std::u16string_view N, JSValue V, bool D);
  virtual JSValue GetBindingValue(std::u16string_view N, bool S) const;
  virtual void DeleteBinding(std::u16string_view N);
  virtual JSValue ImplicitThisValue() const;
};

class DeclarativeEnvironmentRecord : public EnvironmentRecord {
};

class ObjectEnvironmentRecord : public EnvironmentRecord {
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
