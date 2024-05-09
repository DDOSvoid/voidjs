#ifndef VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
#define VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H

#include <unordered_map>

#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/internal_types/binding.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace types {

class HashMap;
class Object;

class EnvironmentRecord : public HeapObject {
 public:
  // Abstract methods of EnvironmentRecord
  // Defined in ECMAScript 5.1 Chapter 10.2.1
  // Only used to forward to concrete method
  static bool HasBinding(VM* vm, JSHandle<EnvironmentRecord> env, JSHandle<String> N);
  static void CreateMutableBinding(VM* vm, JSHandle<EnvironmentRecord> env, JSHandle<String> N, bool D);
  static void SetMutableBinding(VM* vm, JSHandle<EnvironmentRecord> env, JSHandle<String> N, JSHandle<JSValue> V, bool S);
  static JSHandle<JSValue> GetBindingValue(VM* vm, JSHandle<EnvironmentRecord> env, JSHandle<String> N, bool S);
  static bool DeleteBinding(VM* vm, JSHandle<EnvironmentRecord> env, JSHandle<String> N);
  static JSHandle<JSValue> ImplicitThisValue(VM* vm, JSHandle<EnvironmentRecord> env);

  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

class DeclarativeEnvironmentRecord : public EnvironmentRecord {
 public:
  static bool HasBinding(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N);
  static void CreateMutableBinding(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N, bool D);
  static void SetMutableBinding(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N, JSHandle<JSValue> V, bool S);
  static JSHandle<JSValue> GetBindingValue(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N, bool S);
  static bool DeleteBinding(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N);
  static JSHandle<JSValue> ImplicitThisValue(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env);

  // Additional methods of DeclarativeEnvironmentRecord
  static void CreateImmutableBinding(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N);
  static void InitializeImmutableBinding(VM* vm, JSHandle<DeclarativeEnvironmentRecord> env, JSHandle<String> N, JSHandle<JSValue> V);

  // BidningMap* 
  static constexpr std::size_t BINDING_MAP_OFFSET = EnvironmentRecord::END_OFFSET;
  JSValue GetBindingMap() const { return *utils::BitGet<JSValue*>(this, BINDING_MAP_OFFSET); }
  void SetBindingMap(JSValue value) { *utils::BitGet<JSValue*>(this, BINDING_MAP_OFFSET) = value; }
  void SetBindingMap(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, BINDING_MAP_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = EnvironmentRecord::END_OFFSET + SIZE;
};

class ObjectEnvironmentRecord : public EnvironmentRecord {
 public:
  static bool HasBinding(VM* vm, JSHandle<ObjectEnvironmentRecord> env, JSHandle<String> N);
  static void CreateMutableBinding(VM* vm, JSHandle<ObjectEnvironmentRecord> env, JSHandle<String> N, bool D);
  static void SetMutableBinding(VM* vm, JSHandle<ObjectEnvironmentRecord> env, JSHandle<String> N, JSHandle<JSValue> V, bool S);
  static JSHandle<JSValue> GetBindingValue(VM* vm, JSHandle<ObjectEnvironmentRecord> env, JSHandle<String> N, bool S);
  static bool DeleteBinding(VM* vm, JSHandle<ObjectEnvironmentRecord> env, JSHandle<String> N);
  static JSHandle<JSValue> ImplicitThisValue(VM* vm, JSHandle<ObjectEnvironmentRecord> env);

  // Object*
  static constexpr std::size_t OBJECT_OFFSET = EnvironmentRecord::END_OFFSET;
  JSValue GetObject() const { return *utils::BitGet<JSValue*>(this, OBJECT_OFFSET); }
  void SetObject(JSValue value) { *utils::BitGet<JSValue*>(this, OBJECT_OFFSET) = value; }
  void SetObject(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, OBJECT_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = EnvironmentRecord::END_OFFSET + SIZE;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
