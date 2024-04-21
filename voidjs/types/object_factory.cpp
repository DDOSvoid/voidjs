#include "voidjs/types/object_factory.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/internal_types/property_map.h"

namespace voidjs {

using namespace types;

std::byte* ObjectFactory::Allocate(std::size_t size) {
  return new std::byte[size];
}

HeapObject* ObjectFactory::NewHeapObject(std::size_t size) {
  auto obj = reinterpret_cast<HeapObject*>(Allocate(HeapObject::SIZE + size));
  obj->SetMetaData(0);
  return obj;
}

String* ObjectFactory::NewString(std::u16string_view source) {
  auto len = source.size();
  auto str = NewHeapObject(sizeof(std::size_t) + len * sizeof(char16_t))->AsString();
  str->SetType(JSType::STRING);
  str->SetLength(len);
  std::copy(source.begin(), source.end(), str->GetData());
  return str;
}

Object* ObjectFactory::NewObject() {
  auto obj = NewHeapObject(Object::SIZE)->AsObject();
  obj->SetProperties(JSValue(NewPropertyMap()));
  obj->SetProtoType(JSValue::Null());
  return obj;
}

Array* ObjectFactory::NewArray(std::size_t len) {
  auto arr = NewHeapObject(sizeof(std::size_t) + len * sizeof(JSValue))->AsArray();
  arr->SetType(JSType::ARRAY);
  arr->SetLength(len);
  std::fill_n(arr->GetData(), len, JSValue{});
  return arr;
}

DataPropertyDescriptor* ObjectFactory::NewDataPropertyDescriptor(
  const types::PropertyDescriptor &desc) {
  auto prop = NewHeapObject(DataPropertyDescriptor::SIZE)->AsDataPropertyDescriptor();
  prop->SetType(JSType::DATA_PROPERTY_DESCRIPTOR);
  prop->SetValue(desc.GetValue());
  prop->SetWritable(desc.GetWritable());
  prop->SetEnumerable(desc.GetEnumerable());
  prop->SetConfigurable(desc.GetConfigurable());
  return prop;
}

AccessorPropertyDescriptor* ObjectFactory::NewAccessorPropertyDescriptor(
  const types::PropertyDescriptor &desc) {
  auto prop = NewHeapObject(AccessorPropertyDescriptor::SIZE)->AsAccessorPropertyDescriptor();
  prop->SetType(JSType::ACCESSOR_PROPERTY_DESCRIPTOR);
  prop->SetGetter(desc.GetGetter());
  prop->SetSetter(desc.GetSetter());
  prop->SetEnumerable(desc.GetEnumerable());
  prop->SetConfigurable(desc.GetConfigurable());
  return prop;
}

GenericPropertyDescriptor* ObjectFactory::NewGenericPropertyDescriptor(
  const types::PropertyDescriptor &desc) {
  auto prop = NewHeapObject(GenericPropertyDescriptor::SIZE)->AsGenericPropertyDescriptor();
  prop->SetType(JSType::GENERIC_PROPERTY_DESCRIPTOR);
  prop->SetEnumerable(desc.GetEnumerable());
  prop->SetConfigurable(desc.GetConfigurable());
  return prop;
}

PropertyMap* ObjectFactory::NewPropertyMap() {
  auto map = NewArray(2 * PropertyMap::DEFAULT_ENTRY_NUM)->AsPropertyMap();
  map->SetType(JSType::PROPERTY_MAP);
  return map;
}

}  // namespace voidjs
