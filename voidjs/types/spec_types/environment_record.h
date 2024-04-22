#ifndef VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
#define VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H

#include <unordered_map>

#include "voidjs/types/js_value.h"
#include "voidjs/types/internal_types/binding.h"

namespace voidjs {
namespace types {

class EnvironmentRecord {
 public:
  // Abstract methods of EnvironmentRecord
  // Defined in ECMAScript 5.1 Chapter 10.2.1
  virtual bool HasBinding(String* N) const = 0;
  virtual void CreateMutableBinding(String* N, bool D) = 0;
  virtual void SetMutableBinding(String* N, JSValue V, bool S) = 0;
  virtual JSValue GetBindingValue(String* N, bool S) const = 0;
  virtual bool DeleteBinding(String* N) = 0;
  virtual JSValue ImplicitThisValue() const = 0;
};

class DeclarativeEnvironmentRecord : public EnvironmentRecord {
 public:
  // Inherited from base class 
  virtual bool HasBinding(String* N) const override;
  virtual void CreateMutableBinding(String* N, bool D) override;
  virtual void SetMutableBinding(String* N, JSValue V, bool S) override;
  virtual JSValue GetBindingValue(String* N, bool S) const override;
  virtual bool DeleteBinding(String* N) override;
  virtual JSValue ImplicitThisValue() const override;

  // Additional methods of DeclarativeEnvironmentRecord
  void CreateInmmutableBinding(String* N);
  void InitializeImmutableBinding(String* N, JSValue V);

 private:
  std::unordered_map<String*, Binding*> binding_map_;
};

class ObjectEnvironmentRecord : public EnvironmentRecord {
 public:
  // Inherited from base class 
  virtual bool HasBinding(String* N) const override;
  virtual void CreateMutableBinding(String* N, bool D) override;
  virtual void SetMutableBinding(String* N, JSValue V, bool S) override;
  virtual JSValue GetBindingValue(String* N, bool S) const override;
  virtual bool DeleteBinding(String* N) override;
  virtual JSValue ImplicitThisValue() const override;

  bool GetProvideThis() const { return provide_this_; }
  void SetProvideThis(bool flag) { provide_this_ = flag; }

 private:
  bool provide_this_ {false};
  Object* object_; 
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
