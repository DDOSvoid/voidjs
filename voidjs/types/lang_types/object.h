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
  JSValue GetProperties() const { return *utils::BitGet<JSValue*>(this, PROPERTIES_OFFSET); }
  void SetProperties(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, PROPERTIES_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t PROTOTYPE_OFFSET = PROPERTIES_OFFSET + sizeof(JSValue);
  JSValue GetPrototype() const { return *utils::BitGet<JSValue*>(this, PROTOTYPE_OFFSET); }
  void SetPrototype(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, PROTOTYPE_OFFSET) = handle.GetJSValue(); }

  static constexpr std::size_t SIZE = sizeof(JSValue) + sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = PROTOTYPE_OFFSET + sizeof(JSValue);

  // Internal function properties common to all objects
  // Defined in ECMAScript 5.1 Chapter 8.12
  static PropertyDescriptor GetOwnProperty(VM* vm, JSHandle<Object> O, JSHandle<String> P);
  static PropertyDescriptor GetProperty(VM* vm, JSHandle<Object> O, JSHandle<String> P);
  static JSHandle<JSValue> Get(VM* vm, JSHandle<Object> O, JSHandle<String> P);
  static bool CanPut(VM* vm, JSHandle<Object> O, JSHandle<String> P);
  static void Put(VM* vm, JSHandle<Object> O, JSHandle<String> P, JSHandle<JSValue> V, bool Throw);
  static bool HasProperty(VM* vm, JSHandle<Object> O, JSHandle<String> P);
  static bool Delete(VM* vm, JSHandle<Object> O, JSHandle<String> P, bool Throw);
  static JSHandle<JSValue> DefaultValue(VM* vm, JSHandle<Object> O, PreferredType hint);
  static bool DefineOwnProperty(VM* vm, JSHandle<Object> O, JSHandle<String> P, const PropertyDescriptor& Desc, bool Throw);
  static bool DefineOwnPropertyDefault(VM* vm, JSHandle<Object> O, JSHandle<String> P, const PropertyDescriptor& Desc, bool Throw);

  // Internal function properties only defined for some objects
  // Methods defined below ponly used for forwarding
  static JSHandle<JSValue> Construct(JSHandle<Object> O, RuntimeCallInfo* argv);
  static JSHandle<JSValue> Call(JSHandle<Object> O, RuntimeCallInfo* argv);

  static void GetAllEnumerableProps(Object* O);
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANG_TYPES_OBJECT_H
