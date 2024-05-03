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
  static bool HasBinding(VM* vm, EnvironmentRecord* env, String* N);
  static void CreateMutableBinding(VM* vm, EnvironmentRecord* env, String* N, bool D);
  static void SetMutableBinding(VM* vm, EnvironmentRecord* env, String* N, JSValue V, bool S);
  static JSValue GetBindingValue(VM* vm, EnvironmentRecord* env, String* N, bool S);
  static bool DeleteBinding(VM* vm, EnvironmentRecord* env, String* N);
  static JSValue ImplicitThisValue(VM* vm, EnvironmentRecord* env);

  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

class DeclarativeEnvironmentRecord : public EnvironmentRecord {
 public:
  static bool HasBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N);
  static void CreateMutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N, bool D);
  static void SetMutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N, JSValue V, bool S);
  static JSValue GetBindingValue(VM* vm, DeclarativeEnvironmentRecord* env, String* N, bool S);
  static bool DeleteBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N);
  static JSValue ImplicitThisValue(VM* vm, DeclarativeEnvironmentRecord* env);

  // Additional methods of DeclarativeEnvironmentRecord
  static void CreateImmutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N);
  static void InitializeImmutableBinding(VM* vm, DeclarativeEnvironmentRecord* env, String* N, JSValue V);

  static constexpr std::size_t BINDING_MAP_OFFSET = EnvironmentRecord::END_OFFSET;
  HashMap* GetBindingMap() const { return *utils::BitGet<HashMap**>(this, BINDING_MAP_OFFSET); }
  void SetBindingMap(HashMap* map) { *utils::BitGet<HashMap**>(this, BINDING_MAP_OFFSET) = map; }
};

class ObjectEnvironmentRecord : public EnvironmentRecord {
 public:
  static bool HasBinding(VM* vm, ObjectEnvironmentRecord* env, String* N);
  static void CreateMutableBinding(VM* vm, ObjectEnvironmentRecord* env, String* N, bool D);
  static void SetMutableBinding(VM* vm, ObjectEnvironmentRecord* env, String* N, JSValue V, bool S);
  static JSValue GetBindingValue(VM* vm, ObjectEnvironmentRecord* env, String* N, bool S);
  static bool DeleteBinding(VM* vm, ObjectEnvironmentRecord* env, String* N);
  static JSValue ImplicitThisValue(VM* vm, ObjectEnvironmentRecord* env);

  static constexpr std::size_t OBJECT_OFFSET = EnvironmentRecord::END_OFFSET;
  Object* GetObject() const { return *utils::BitGet<Object**>(this, OBJECT_OFFSET); }
  void SetObject(Object* obj) { *utils::BitGet<Object**>(this, OBJECT_OFFSET) = obj; }

};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_ENVIRONMENT_RECORD_H
