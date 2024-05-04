#ifndef VOIDJS_TYPES_OBJECT_FACTORY_H
#define VOIDJS_TYPES_OBJECT_FACTORY_H

#include <string>
#include <cstdint>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {

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

  RuntimeCallInfo* NewRuntimeCallInfo(JSValue this_arg, const std::vector<JSValue>& args);

  // used to create builtin objects
  types::Object* NewEmptyObject(
    std::size_t extra_size, JSType type, ObjectClassType class_type,
    JSValue proto, bool extensible, bool callable, bool is_counstructor);
  
  types::String* NewString(std::u16string_view source);
  types::String* NewStringFromTable(std::u16string_view source);
  types::Object* NewObject(
    std::size_t extra_size, JSType type, ObjectClassType class_type, JSValue proto,
    bool extensible, bool callable, bool is_counstructor);
  types::Array* NewArray(std::size_t len);
  types::DataPropertyDescriptor* NewDataPropertyDescriptor(const types::PropertyDescriptor& desc);
  types::AccessorPropertyDescriptor* NewAccessorPropertyDescriptor(const types::PropertyDescriptor& desc);
  types::GenericPropertyDescriptor* NewGenericPropertyDescriptor(const types::PropertyDescriptor& desc);
  types::PropertyMap* NewPropertyMap();
  types::Binding* NewBinding(JSValue value, bool _mutable, bool deletable);
  types::InternalFunction* NewInternalFunction(InternalFunctionType func);
  types::HashMap* NewHashMap(std::uint32_t capacity);
  types::EnvironmentRecord* NewEnvironmentRecord();
  types::DeclarativeEnvironmentRecord* NewDeclarativeEnvironmentRecord();
  types::ObjectEnvironmentRecord* NewObjectEnvironmentRecord(types::Object* obj);
  types::LexicalEnvironment* NewLexicalEnvironment(types::LexicalEnvironment* outer, types::EnvironmentRecord* env_rec);

  builtins::JSObject* NewJSObject(JSValue value);
  builtins::JSFunction* NewJSFunction(JSValue value);
  builtins::JSError* NewJSError(types::String* msg);
  builtins::JSError* NewNativeError(ErrorType type, types::String* msg);  
  builtins::JSError* NewNativeError(ErrorType type, std::u16string_view msg);  

 private:
  VM* vm_;
  StringTable* string_table_;
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_OBJECT_FACTORY_H
