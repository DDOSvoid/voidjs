#ifndef VOIDJS_TYPES_OBJECT_FACTORY_H
#define VOIDJS_TYPES_OBJECT_FACTORY_H

#include <string>
#include <cstdint>

#include "voidjs/types/heap_object.h"
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

class HeapObject;

class ObjectFactory {
 public:
  static std::byte* Allocate(std::size_t size);
  
  static HeapObject* NewHeapObject(std::size_t size);
  static types::String* NewString(std::u16string_view source);
  static types::Object* NewObject();
  static types::Array* NewArray(std::size_t len);
  static types::DataPropertyDescriptor* NewDataPropertyDescriptor(const types::PropertyDescriptor& desc);
  static types::AccessorPropertyDescriptor* NewAccessorPropertyDescriptor(const types::PropertyDescriptor& desc);
  static types::GenericPropertyDescriptor* NewGenericPropertyDescriptor(const types::PropertyDescriptor& desc);
  static types::PropertyMap* NewPropertyMap();
  static types::Binding* NewBinding(JSValue value, bool _mutable, bool deletable); 
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_OBJECT_FACTORY_H
