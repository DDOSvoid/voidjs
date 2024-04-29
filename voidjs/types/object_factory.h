#ifndef VOIDJS_TYPES_OBJECT_FACTORY_H
#define VOIDJS_TYPES_OBJECT_FACTORY_H

#include <string>
#include <cstdint>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/property_descriptor.h"

namespace voidjs {

namespace types {

class String;
class Object;
class Array;
class DataPropertyDescriptor;
class AccessorPropertyDescriptor;
class PropertyMap;

}  // namespace types

namespace builtins {

class GlobalObject;
class JSObject;

}  // namespace builtins

class HeapObject;
class RuntimeCallInfo;
class StringTable;

class ObjectFactory {
 public:
  ObjectFactory(VM* vm, StringTable* string_table)
    : vm_(vm), string_table_(string_table)
  {}
  
  std::byte* Allocate(std::size_t size);

  builtins::GlobalObject* NewGlobalObject();
  
  HeapObject* NewHeapObject(std::size_t size);

  RuntimeCallInfo* NewRuntimeCallInfo(std::size_t args_num);
  
  types::String* NewString(std::u16string_view source);
  types::String* NewStringFromTable(std::u16string_view source);
  types::Object* NewObject(JSValue proto);
  types::Array* NewArray(std::size_t len);
  types::DataPropertyDescriptor* NewDataPropertyDescriptor(const types::PropertyDescriptor& desc);
  types::AccessorPropertyDescriptor* NewAccessorPropertyDescriptor(const types::PropertyDescriptor& desc);
  types::GenericPropertyDescriptor* NewGenericPropertyDescriptor(const types::PropertyDescriptor& desc);
  types::PropertyMap* NewPropertyMap();
  types::Binding* NewBinding(JSValue value, bool _mutable, bool deletable);

  types::Object* NewEmptyObject(std::size_t extra_size);
  builtins::JSObject* NewJSObject(JSValue value);
  builtins::JSFunction* NewJSFunction(JSValue value);

 private:
  VM* vm_;
  StringTable* string_table_;
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_OBJECT_FACTORY_H
