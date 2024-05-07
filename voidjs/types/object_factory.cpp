#include "voidjs/types/object_factory.h"

#include <functional>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/binding.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/types/internal_types/hash_map.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {

std::byte* ObjectFactory::Allocate(std::size_t size) {
  return new std::byte[size];
}

// Unless otherwise specified, the standard built-in properties of the global object have
// attributes {[[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: true}.
// 
// The global object does not have a [[Construct]] internal property;
// it is not possible to use the global object as a constructor with the new operator.
// 
// The global object does not have a [[Call]] internal property;
// it is not possible to invoke the global object as a function.
// 
// The values of the [[Prototype]] and [[Class]]
// internal properties of the global object are implementation-dependent.
// 
// Properties of Global Object are initialized in Interpreter::InitializeBuiltinObjects()
JSHandle<builtins::GlobalObject> ObjectFactory::NewGlobalObject() {
  auto obj = NewHeapObject(types::Object::SIZE).As<builtins::GlobalObject>();
  obj->SetType(JSType::GLOBAL_OBJECT);
  obj->SetClassType(ObjectClassType::GLOBAL_OBJECT);
  obj->SetProperties(NewPropertyMap().As<JSValue>());
  obj->SetExtensible(true);
  obj->SetCallable(false);
  obj->SetIsConstructor(false);
  obj->SetPrototype(JSHandle<JSValue>{vm_, JSValue::Null()});
  return obj;
}

JSHandle<HeapObject> ObjectFactory::NewHeapObject(std::size_t size) {
  auto obj = reinterpret_cast<HeapObject*>(Allocate(HeapObject::SIZE + size));
  obj->SetMetaData(0);
  return JSHandle<HeapObject>(vm_, obj);
}

// used to create builtin objects
JSHandle<types::Object> ObjectFactory::NewEmptyObject(
  std::size_t extra_size, JSType type, ObjectClassType class_type, JSValue proto,
  bool extensible, bool callable, bool is_counstructor) {
}

JSHandle<types::String> ObjectFactory::NewString(std::u16string_view source) {
  auto len = source.size();
  auto str = NewHeapObject(sizeof(std::size_t) + len * sizeof(char16_t)).As<types::String>();
  str->SetType(JSType::STRING);
  str->SetLength(len);
  std::copy(source.begin(), source.end(), str->GetData());
  return str;
}

JSHandle<types::String> ObjectFactory::GetStringFromTable(std::u16string_view str_view) {
  return string_table_->GetOrInsert(str_view);
}

JSHandle<types::String> ObjectFactory::GetEmptyString() {
  return string_table_->GetOrInsert(u"");
}

JSHandle<types::String> ObjectFactory::GetLengthString() {
  return string_table_->GetOrInsert(u"length");
}

JSHandle<types::String> ObjectFactory::GetIntString(std::int32_t i) {
  return JSValue::NumberToString(vm_, i);
}

JSHandle<types::Object> ObjectFactory::NewObject(
  std::size_t extra_size, JSType type, ObjectClassType class_type,
  JSHandle<JSValue> proto, bool extensible, bool callable, bool is_counstructor) {
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

JSHandle<types::Array> ObjectFactory::NewArray(std::size_t len) {
  auto arr = NewHeapObject(sizeof(std::size_t) + len * sizeof(JSValue)).As<types::Array>();
  arr->SetType(JSType::ARRAY);
  arr->SetLength(len);
  std::fill_n(arr->GetData(), len, JSValue{});
  return arr;
}

JSHandle<types::DataPropertyDescriptor> ObjectFactory::NewDataPropertyDescriptor(
  const types::PropertyDescriptor &desc) {
  auto prop = NewHeapObject(types::DataPropertyDescriptor::SIZE).As<types::DataPropertyDescriptor>();
  prop->SetType(JSType::DATA_PROPERTY_DESCRIPTOR);
  prop->SetValue(desc.GetValue());
  prop->SetWritable(desc.GetWritable());
  prop->SetEnumerable(desc.GetEnumerable());
  prop->SetConfigurable(desc.GetConfigurable());
  return prop;
}

JSHandle<types::AccessorPropertyDescriptor> ObjectFactory::NewAccessorPropertyDescriptor(
  const types::PropertyDescriptor &desc) {
  auto prop = NewHeapObject(types::AccessorPropertyDescriptor::SIZE).As<types::AccessorPropertyDescriptor>();
  prop->SetType(JSType::ACCESSOR_PROPERTY_DESCRIPTOR);
  prop->SetGetter(desc.GetGetter());
  prop->SetSetter(desc.GetSetter());
  prop->SetEnumerable(desc.GetEnumerable());
  prop->SetConfigurable(desc.GetConfigurable());
  return prop;
}

JSHandle<types::GenericPropertyDescriptor> ObjectFactory::NewGenericPropertyDescriptor(
  const types::PropertyDescriptor &desc) {
  auto prop = NewHeapObject(types::GenericPropertyDescriptor::SIZE).As<types::GenericPropertyDescriptor>();
  prop->SetType(JSType::GENERIC_PROPERTY_DESCRIPTOR);
  prop->SetEnumerable(desc.GetEnumerable());
  prop->SetConfigurable(desc.GetConfigurable());
  return prop;
}

JSHandle<types::PropertyMap> ObjectFactory::NewPropertyMap() {
  auto prop_map = NewHashMap(types::PropertyMap::DEFAULT_PROPERTY_NUMS).As<types::PropertyMap>();
  prop_map->SetType(JSType::PROPERTY_MAP);
  return prop_map;
}

JSHandle<types::Binding> ObjectFactory::NewBinding(JSHandle<JSValue> value, bool _mutable, bool deletable) {
  auto binding = NewHeapObject(types::Binding::SIZE).As<types::Binding>();
  binding->SetType(JSType::BINDING);
  binding->SetMutable(_mutable);
  binding->SetDeletable(deletable);
  binding->SetValue(value);
  return binding;
}

JSHandle<types::InternalFunction> ObjectFactory::NewInternalFunction(InternalFunctionType func) {
  auto internal_func = NewHeapObject(types::InternalFunction::SIZE).As<types::InternalFunction>();
  internal_func->SetType(JSType::INTERNAL_FUNCTION);
  internal_func->SetFunction(func);
  internal_func->SetCallable(true);
  return internal_func;
}


JSHandle<types::HashMap> ObjectFactory::NewHashMap(std::uint32_t capacity) {
  auto hashmap = NewArray(types::HashMap::HEADER_SIZE + types::HashMap::ENTRY_SIZE * capacity).As<types::HashMap>();
  hashmap->SetType(JSType::HASH_MAP);
  hashmap->SetBucketCapacity(capacity);
  hashmap->SetBucketSize(0);
  return hashmap;
}

JSHandle<types::DeclarativeEnvironmentRecord> ObjectFactory::NewDeclarativeEnvironmentRecord() {
  auto env_rec = NewHeapObject(types::DeclarativeEnvironmentRecord::SIZE).As<types::DeclarativeEnvironmentRecord>();
  env_rec->SetType(JSType::DECLARATIVE_ENVIRONMENT_RECORD);
  env_rec->SetBindingMap(NewHashMap(types::HashMap::MIN_CAPACITY).As<JSValue>());
  return env_rec;
}

JSHandle<types::ObjectEnvironmentRecord> ObjectFactory::NewObjectEnvironmentRecord(JSHandle<types::Object> obj) {
  auto env_rec = NewHeapObject(types::ObjectEnvironmentRecord::SIZE).As<types::ObjectEnvironmentRecord>();
  env_rec->SetType((JSType::OBJECT_ENVIRONMENT_RECORD));
  env_rec->SetObject(obj.As<JSValue>());
  env_rec->SetProvideThis(false);
  return env_rec;
}

JSHandle<types::LexicalEnvironment> ObjectFactory::NewLexicalEnvironment(
  JSHandle<types::LexicalEnvironment> outer, JSHandle<types::EnvironmentRecord> env_rec) {
  auto env = NewHeapObject(types::LexicalEnvironment::SIZE).As<types::LexicalEnvironment>();
  env->SetType(JSType::LEXICAL_ENVIRONMENT);
  env->SetOuter(outer.As<JSValue>());
  env->SetEnvRec(env_rec.As<JSValue>());
  return env;
}

JSHandle<builtins::JSError> ObjectFactory::NewNativeError(ErrorType type, JSHandle<types::String> msg) {
  auto proto = JSHandle<builtins::JSError>{};
  switch (type) {
    case ErrorType::EVAL_ERROR: {
      proto = vm_->GetEvalErrorPrototype();
      break;
    }
    case ErrorType::RANGE_ERROR: {
      proto = vm_->GetRangeErrorPrototype();
      break;
    }
    case ErrorType::REFERENCE_ERROR: {
      proto = vm_->GetReferenceErrorPrototype();
      break;
    }
    case ErrorType::SYNTAX_ERROR: {
      proto = vm_->GetSyntaxErrorPrototype();
      break;
    }
    case ErrorType::TYPE_ERROR: {
      proto = vm_->GetTypeErrorPrototype();
      break;
    }
    case ErrorType::URI_ERROR: {
      proto = vm_->GetURIErrorPrototype();
      break;
    }
    default: {
      break;
    }
  }

  auto error = NewObject(builtins::JSError::SIZE, JSType::JS_ERROR,
                         ObjectClassType::ERROR,
                         proto.As<JSValue>(), true, false, false).As<builtins::JSError>();
  return error;
}

JSHandle<builtins::JSError> ObjectFactory::NewNativeError(ErrorType type, std::u16string_view msg) {
  return NewNativeError(type, GetStringFromTable(msg));
}

}  // namespace voidjs
