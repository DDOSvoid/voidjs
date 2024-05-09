#ifndef VOIDJS_TYPES_OBJECT_FACTORY_H
#define VOIDJS_TYPES_OBJECT_FACTORY_H

#include <string>
#include <cstdint>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/gc/heap.h"
#include "voidjs/gc/js_handle.h"

namespace voidjs {

class HeapObject;
class RuntimeCallInfo;
class StringTable;

class ObjectFactory {
 public:
  ObjectFactory(VM* vm, Heap* heap, StringTable* string_table)
    : vm_(vm), heap_(heap), string_table_(string_table)
  {}

  ~ObjectFactory();
  
  template <GCFlag flag = GCFlag::NORMAL> 
  std::uintptr_t Allocate(std::size_t size) {
    return heap_->Allocate<flag>(size);
  }

  JSHandle<builtins::GlobalObject> NewGlobalObject();

  template <GCFlag flag = GCFlag::NORMAL>
  JSHandle<HeapObject> NewHeapObject(std::size_t size) {
    auto obj = reinterpret_cast<HeapObject*>(Allocate(HeapObject::SIZE + size));
    obj->SetMetaData(0);
    return JSHandle<HeapObject>(vm_, obj);
  }

  // used to create builtin objects
  JSHandle<types::Object> NewEmptyObject(
    std::size_t extra_size, JSType type, ObjectClassType class_type,
    JSValue proto, bool extensible, bool callable, bool is_counstructor);

  template <GCFlag flag = GCFlag::NORMAL>
  JSHandle<types::String> NewString(std::u16string_view source) {
    auto len = source.size();
    auto str = NewHeapObject(sizeof(std::size_t) + len * sizeof(char16_t)).As<types::String>();
    str->SetType(JSType::STRING);
    str->SetLength(len);
    std::copy(source.begin(), source.end(), str->GetData());
    return str;
  }
  JSHandle<types::String> NewStringFromInt(std::int32_t i);

  template <GCFlag flag = GCFlag::NORMAL>
  JSHandle<types::Object> NewObject(
    std::size_t extra_size, JSType type, ObjectClassType class_type, JSHandle<JSValue> proto,
    bool extensible, bool callable, bool is_counstructor) {
    auto obj = NewHeapObject(types::Object::SIZE + extra_size).As<types::Object>();

    obj->SetType(type);
    obj->SetClassType(class_type);
    obj->SetProperties(NewPropertyMap().As<JSValue>());
    obj->SetPrototype(proto);
    obj->SetExtensible(extensible);
    obj->SetCallable(callable);
    obj->SetIsConstructor(is_counstructor);
  
    return obj;
  }
  JSHandle<types::Array> NewArray(std::size_t len);
  JSHandle<types::DataPropertyDescriptor> NewDataPropertyDescriptor(const types::PropertyDescriptor& desc);
  JSHandle<types::AccessorPropertyDescriptor> NewAccessorPropertyDescriptor(const types::PropertyDescriptor& desc);
  JSHandle<types::GenericPropertyDescriptor> NewGenericPropertyDescriptor(const types::PropertyDescriptor& desc);
  JSHandle<types::PropertyMap> NewPropertyMap();
  JSHandle<types::Binding> NewBinding(JSHandle<JSValue> value, bool _mutable, bool deletable);
  JSHandle<types::InternalFunction> NewInternalFunction(InternalFunctionType func);
  JSHandle<types::HashMap> NewHashMap(std::uint32_t capacity);
  JSHandle<types::EnvironmentRecord> NewEnvironmentRecord();
  JSHandle<types::DeclarativeEnvironmentRecord> NewDeclarativeEnvironmentRecord();
  JSHandle<types::ObjectEnvironmentRecord> NewObjectEnvironmentRecord(JSHandle<types::Object> obj);
  JSHandle<types::LexicalEnvironment> NewLexicalEnvironment(
    JSHandle<types::LexicalEnvironment> outer, JSHandle<types::EnvironmentRecord> env_rec);

  JSHandle<builtins::JSObject> NewJSObject(JSValue value);
  JSHandle<builtins::JSFunction> NewJSFunction(JSValue value);
  JSHandle<builtins::JSError> NewJSError(JSHandle<types::String> msg);
  JSHandle<builtins::JSError> NewNativeError(ErrorType type, JSHandle<types::String> msg);  
  JSHandle<builtins::JSError> NewNativeError(ErrorType type, std::u16string_view msg);  

 private:
  VM* vm_;
  Heap* heap_;
  StringTable* string_table_;
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_OBJECT_FACTORY_H
