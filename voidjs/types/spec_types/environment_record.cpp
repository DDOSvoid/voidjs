#include "voidjs/types/spec_types/environment_record.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace types {

// HasBinding
// Defined in ECMAScript 5.1 Chapter 10.2.1.1.1
bool DeclarativeEnvironmentRecord::HasBinding(String* N) const {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. If envRec has a binding for the name that is the value of N, return true.
  // 3. If it does not have such a binding, return false
  return binding_map_.find(N) != binding_map_.end();
}

void DeclarativeEnvironmentRecord::CreateMutableBinding(String* N, bool D) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec does not already have a binding for N.
  // 3. Create a mutable binding in envRec for N and set its bound value to undefined.
  //    If D is true record that the newly created binding may be deleted by a subsequent DeleteBinding call.
  auto binding = ObjectFactory::NewBinding(JSValue::Undefined(), true, D);
  binding_map_.emplace(N, binding);
}

void DeclarativeEnvironmentRecord::SetMutableBinding(String* N, JSValue V, bool S) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec must have a binding for N.
  // 3. If the binding for N in envRec is a mutable binding, change its bound value to V.
  // 4. Else this must be an attempt to change the value of an immutable binding
  //    so if S is true throw a TypeError exception.
  auto& binding = binding_map_[N];
  if (binding->GetMutable()) {
    binding->SetValue(V);
  } else {
    if (S) {
      // todo
    }
  }
}

JSValue DeclarativeEnvironmentRecord::GetBindingValue(String* N, bool S) const {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec has a binding for N.
  // 3. If the binding for N in envRec is an uninitialized immutable binding, then
  //    a. If S is false, return the value undefined, otherwise throw a ReferenceError exception.
  // 4. Else, return the value currently bound to N in envRec.
  auto binding = binding_map_.at(N);
  if (binding->GetValue().IsUndefined() && !binding->GetMutable()) {
    if (S) {
      // todo
    } else {
      return JSValue::Undefined();
    }
  } else {
    return binding->GetValue();
  }
}

bool DeclarativeEnvironmentRecord::DeleteBinding(String* N) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. If envRec does not have a binding for the name that is the value of N, return true.
  // 3. If the binding for N in envRec is cannot be deleted, return false.
  // 4. Remove the binding for N from envRec.
  // 5. Return true.
  if (binding_map_.find(N) == binding_map_.end()) {
    return true;
  }

  auto binding = binding_map_[N];
  if (!binding->GetDeletable()) {
    return false;
  }

  binding_map_.erase(N);

  return true;
}

JSValue DeclarativeEnvironmentRecord::ImplicitThisValue() const {
  // 1. Return undefined.
  return JSValue::Undefined();
}

void DeclarativeEnvironmentRecord::CreateInmmutableBinding(String* N) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec does not already have a binding for N.
  // 3. Create an immutable binding in envRec for N and record that it is uninitialized.
  auto binding = ObjectFactory::NewBinding(JSValue::Undefined(), false, false);
  binding_map_.emplace(N, binding);
}

void DeclarativeEnvironmentRecord::InitializeImmutableBinding(String* N, JSValue V) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec must have an uninitialized immutable binding for N.
  // 3. Set the bound value for N in envRec to V.
  // 4. Record that the immutable binding for N in envRec has been initialized.
  auto& binding = binding_map_[N];
  binding->SetValue(V);
}


bool ObjectEnvironmentRecord::HasBinding(String *N) const {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Return the result of calling the [[HasProperty]] internal method of bindings, passing N as the property name.
  return object_->HasProperty(JSValue(N));
}

void ObjectEnvironmentRecord::CreateMutableBinding(String* N, bool D) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Assert: The result of calling the [[HasProperty]] internal method of bindings, passing N as the property name, is false.
  // 4. If D is true then let configValue be true otherwise let configValue be false.
  auto config_value = D;
  
  // 5. Call the [[DefineOwnProperty]] internal method of bindings,
  //    passing N, Property Descriptor {[[Value]]:undefined, [[Writable]]: true,
  //    [[Enumerable]]: true , [[Configurable]]: configValue}, and true as arguments.
  object_->DefineOwnProperty(JSValue(N),
                             PropertyDescriptor(JSValue::Undefined(), true, true, config_value),
                             true);
}

void ObjectEnvironmentRecord::SetMutableBinding(String* N, JSValue V, bool S) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Call the [[Put]] internal method of bindings with arguments N, V, and S.
  object_->Put(JSValue(N), V, S);
}

JSValue ObjectEnvironmentRecord::GetBindingValue(String* N, bool S) const {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Let value be the result of calling the [[HasProperty]] internal method of bindings, passing N as the property name.
  auto value = object_->HasProperty(JSValue(N));
  
  // 4. If value is false, then
  if (!value) {
    // a. If S is false, return the value undefined, otherwise throw a ReferenceError exception.
    if (S) {
      // todo
    } else {
      return JSValue::Undefined();
    }
  }
  
  // 5. Return the result of calling the [[Get]] internal method of bindings, passing N for the argument.
  return object_->Get(JSValue(N));
}

bool ObjectEnvironmentRecord::DeleteBinding(String *N) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Return the result of calling the [[Delete]] internal method of bindings, passing N and false as arguments.
  return object_->Delete(JSValue(N), false);
}

JSValue ObjectEnvironmentRecord::ImplicitThisValue() const {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. If the provideThis flag of envRec is true, return the binding object for envRec.
  if (provide_this_) {
    return JSValue(object_);
  }
  // 3. Otherwise, return undefined.
  else {
    return JSValue::Undefined();
  }
}
  
}  // namespace types
}  // namespace voidjs
