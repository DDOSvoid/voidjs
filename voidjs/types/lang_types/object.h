#ifndef VOIDJS_TYPES_LANG_TYPES_OBJECT_H
#define VOIDJS_TYPES_LANG_TYPES_OBJECT_H

#include <cstddef>
#include <cstdint>
#include <optional>

#include "voidjs/types/object_factory.h"
#include "voidjs/utils/helper.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/interpreter/vm.h"

namespace voidjs {
namespace types {

class String;

// properties
// prototype
class Object : public HeapObject {
 public:
  static constexpr std::size_t PROPERTIES_OFFSET = HeapObject::SIZE;
  PropertyMap* GetPropertyMap() const { return *utils::BitGet<PropertyMap**>(this, PROPERTIES_OFFSET); }
  void SetPropertyMap(PropertyMap* prop_map) { *utils::BitGet<PropertyMap**>(this, PROPERTIES_OFFSET) = prop_map; }

  static constexpr std::size_t PROTOTYPE_OFFSET = PROPERTIES_OFFSET + sizeof(JSValue);
  JSValue GetPrototype() const { return *utils::BitGet<JSValue*>(this, PROTOTYPE_OFFSET); }
  void SetPrototype(JSValue proto) { *utils::BitGet<JSValue*>(this, PROTOTYPE_OFFSET) = proto; }

  static constexpr std::size_t SIZE = sizeof(JSValue) + sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = PROTOTYPE_OFFSET + sizeof(JSValue);

  // Internal function properties common to all objects
  // Defined in ECMAScript 5.1 Chapter 8.12
  static PropertyDescriptor GetOwnProperty(VM* vm, Object* O, String* P);
  static PropertyDescriptor GetProperty(VM* vm, Object* O, String* P);
  static JSValue Get(VM* vm, Object* O, String* P);
  static bool CanPut(VM* vm, Object* O, String* P);
  static void Put(VM* vm, Object* O, String* P, JSValue V, bool Throw);
  static bool HasProperty(VM* vm, Object* O, String* P);
  static bool Delete(VM* vm, Object* O, String* P, bool Throw);
  static JSValue DefaultValue(VM* vm, Object* O, PreferredType hint);
  static bool DefineOwnProperty(VM* vm, Object* O, String* P, const PropertyDescriptor& Desc, bool Throw);

  // Internal function properties only defined for some objects
  // Methods defined below ponly used for forwarding
  static JSValue Construct(Object* O, RuntimeCallInfo* argv);
  static JSValue Call(Object* O, RuntimeCallInfo* argv);

  static void GetAllEnumerableProps(Object* O);
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANG_TYPES_OBJECT_H
