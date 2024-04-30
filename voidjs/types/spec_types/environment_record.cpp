#include "voidjs/types/spec_types/environment_record.h"

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/internal_types/hash_map.h"

namespace voidjs {
namespace types {

// HasBinding
// Only used to forward to concrete method
bool EnvironmentRecord::HasBinding(VM* vm, EnvironmentRecord* env, String* N) {
  if (env->IsDeclarativeEnvironmentRecord()) {
    return DeclarativeEnvironmentRecord::HasBinding(vm, env->AsDeclarativeEnvironmentRecord(), N);
  } else {
    // env must be ObjectEnvironmentRecord
    return ObjectEnvironmentRecord::HasBinding(vm, env->AsObjectEnvironmentRecord(), N);
  }
}

// CreateMutableBinding
// Only used to forward to concrete method
void EnvironmentRecord::CreateMutableBinding(VM* vm, EnvironmentRecord* env, String* N, bool D) {
  if (env->IsDeclarativeEnvironmentRecord()) {
    DeclarativeEnvironmentRecord::CreateMutableBinding(vm, env->AsDeclarativeEnvironmentRecord(), N, D);
  } else {
    // env must be ObjectEnvironmentRecord
    ObjectEnvironmentRecord::CreateMutableBinding(vm, env->AsObjectEnvironmentRecord(), N, D);
  }
}

// SetMutableBinding
// Only used to forward to concrete method
void EnvironmentRecord::SetMutableBinding(VM* vm, EnvironmentRecord* env, String* N, JSValue V, bool S) {
  if (env->IsDeclarativeEnvironmentRecord()) {
    DeclarativeEnvironmentRecord::SetMutableBinding(vm, env->AsDeclarativeEnvironmentRecord(), N, V, S);
  } else {
    // env must be ObjectEnvironmentRecord
    ObjectEnvironmentRecord::SetMutableBinding(vm, env->AsObjectEnvironmentRecord(), N, V, S);
  }
}

// GetBindingValue
// Only used to forward to concrete method
JSValue EnvironmentRecord::GetBindingValue(VM* vm, EnvironmentRecord* env, String* N, bool S) {
  if (env->IsDeclarativeEnvironmentRecord()) {
    return DeclarativeEnvironmentRecord::GetBindingValue(vm, env->AsDeclarativeEnvironmentRecord(), N, S);
  } else {
    // env must be ObjectEnvironmentRecord
    return ObjectEnvironmentRecord::GetBindingValue(vm, env->AsObjectEnvironmentRecord(), N, S);
  }
}

// DeleteBinding
// Only used to forward to concrete method
bool EnvironmentRecord::DeleteBinding(VM* vm, EnvironmentRecord* env, String* N) {
  if (env->IsDeclarativeEnvironmentRecord()) {
    return DeclarativeEnvironmentRecord::DeleteBinding(vm, env->AsDeclarativeEnvironmentRecord(), N);
  } else {
    // env must be ObjectEnvironmentRecord
    return ObjectEnvironmentRecord::DeleteBinding(vm, env->AsObjectEnvironmentRecord(), N);
  }
}

// ImplicitThisValue
// Only used to forward to concrete method
JSValue EnvironmentRecord::ImplicitThisValue(VM* vm, EnvironmentRecord* env) {
  if (env->IsDeclarativeEnvironmentRecord()) {
    return DeclarativeEnvironmentRecord::ImplicitThisValue(vm, env->AsDeclarativeEnvironmentRecord());
  } else {
    // env must be ObjectEnvironmentRecord
    return ObjectEnvironmentRecord::ImplicitThisValue(vm, env->AsObjectEnvironmentRecord());
  }
}

// HasBinding
// Defined in ECMAScript 5.1 Chapter 10.2.1.1.1
bool DeclarativeEnvironmentRecord::HasBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. If envRec has a binding for the name that is the value of N, return true.
  // 3. If it does not have such a binding, return false
  return !env->GetBindingMap()->Find(vm, N).IsEmpty();
}

void DeclarativeEnvironmentRecord::CreateMutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N, bool D) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec does not already have a binding for N.
  // 3. Create a mutable binding in envRec for N and set its bound value to undefined.
  //    If D is true record that the newly created binding may be deleted by a subsequent DeleteBinding call.
  auto binding = vm->GetObjectFactory()->NewBinding(JSValue::Undefined(), true, D);
  env->SetBindingMap(HashMap::Insert(vm, env->GetBindingMap(), N, JSValue(binding)));
}

void DeclarativeEnvironmentRecord::SetMutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N, JSValue V, bool S) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec must have a binding for N.
  // 3. If the binding for N in envRec is a mutable binding, change its bound value to V.
  // 4. Else this must be an attempt to change the value of an immutable binding
  //    so if S is true throw a TypeError exception.
  auto binding = env->GetBindingMap()->Find(vm, N).GetHeapObject()->AsBinding();

  if (binding->GetMutable()) {
    binding->SetValue(V);
  } else {
    if (S) {
      // todo
    }
  }

  env->SetBindingMap(HashMap::Insert(vm, env->GetBindingMap(), N, JSValue(binding)));
}

JSValue DeclarativeEnvironmentRecord::GetBindingValue(VM* vm, DeclarativeEnvironmentRecord* env, String* N, bool S) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec has a binding for N.
  // 3. If the binding for N in envRec is an uninitialized immutable binding, then
  //    a. If S is false, return the value undefined, otherwise throw a ReferenceError exception.
  // 4. Else, return the value currently bound to N in envRec.
  auto binding = env->GetBindingMap()->Find(vm, N).GetHeapObject()->AsBinding();
  
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

bool DeclarativeEnvironmentRecord::DeleteBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. If envRec does not have a binding for the name that is the value of N, return true.
  // 3. If the binding for N in envRec is cannot be deleted, return false.
  // 4. Remove the binding for N from envRec.
  // 5. Return true.

  auto binding = env->GetBindingMap()->Find(vm, N).GetHeapObject()->AsBinding();
  
  if (!binding->GetDeletable()) {
    return false;
  }

  env->GetBindingMap()->Erase(vm, N);

  return true;
}

JSValue DeclarativeEnvironmentRecord::ImplicitThisValue(VM* vm, DeclarativeEnvironmentRecord* env) {
  // 1. Return undefined.
  return JSValue::Undefined();
}

void DeclarativeEnvironmentRecord::CreateInmmutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec does not already have a binding for N.
  // 3. Create an immutable binding in envRec for N and record that it is uninitialized.
  auto binding = vm->GetObjectFactory()->NewBinding(JSValue::Undefined(), false, false);
  env->SetBindingMap(HashMap::Insert(vm, env->GetBindingMap(), N, JSValue(binding)));
}

void DeclarativeEnvironmentRecord::InitializeImmutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N, JSValue V) {
  // 1. Let envRec be the declarative environment record for which the method was invoked.
  // 2. Assert: envRec must have an uninitialized immutable binding for N.
  // 3. Set the bound value for N in envRec to V.
  // 4. Record that the immutable binding for N in envRec has been initialized.
  auto binding = env->GetBindingMap()->Find(vm, N).GetHeapObject()->AsBinding();

  binding->SetValue(V);

  env->SetBindingMap(HashMap::Insert(vm, env->GetBindingMap(), N, JSValue(binding)));
}


bool ObjectEnvironmentRecord::HasBinding(VM* vm, ObjectEnvironmentRecord* env, String* N) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Return the result of calling the [[HasProperty]] internal method of bindings, passing N as the property name.
  return Object::HasProperty(vm, env->GetObject(), N);
}

void ObjectEnvironmentRecord::CreateMutableBinding(VM* vm, ObjectEnvironmentRecord* env, String* N, bool D) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Assert: The result of calling the [[HasProperty]] internal method of bindings, passing N as the property name, is false.
  // 4. If D is true then let configValue be true otherwise let configValue be false.
  auto config_value = D;
  
  // 5. Call the [[DefineOwnProperty]] internal method of bindings,
  //    passing N, Property Descriptor {[[Value]]:undefined, [[Writable]]: true,
  //    [[Enumerable]]: true , [[Configurable]]: configValue}, and true as arguments.
  Object::DefineOwnProperty(vm, env->GetObject(), N,
                             PropertyDescriptor(JSValue::Undefined(), true, true, config_value),
                             true);
}

void ObjectEnvironmentRecord::SetMutableBinding(VM* vm, ObjectEnvironmentRecord* env, String* N, JSValue V, bool S) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Call the [[Put]] internal method of bindings with arguments N, V, and S.
  Object::Put(vm, env->GetObject(), N, V, S);
}

JSValue ObjectEnvironmentRecord::GetBindingValue(VM* vm, ObjectEnvironmentRecord* env, String* N, bool S) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Let value be the result of calling the [[HasProperty]] internal method of bindings, passing N as the property name.
  auto value = Object::HasProperty(vm, env->GetObject(), N);
  
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
  return Object::Get(vm, env->GetObject(), N);
}

bool ObjectEnvironmentRecord::DeleteBinding(VM* vm, ObjectEnvironmentRecord* env, String *N) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. Let bindings be the binding object for envRec.
  // 3. Return the result of calling the [[Delete]] internal method of bindings, passing N and false as arguments.
  return Object::Delete(vm, env->GetObject(), N, false);
}

JSValue ObjectEnvironmentRecord::ImplicitThisValue(VM* vm, ObjectEnvironmentRecord* env) {
  // 1. Let envRec be the object environment record for which the method was invoked.
  // 2. If the provideThis flag of envRec is true, return the binding object for envRec.
  if (env->GetProvideThis()) {
    return JSValue(env->GetObject());
  }
  // 3. Otherwise, return undefined.
  else {
    return JSValue::Undefined();
  }
}
  
}  // namespace types
}  // namespace voidjs
