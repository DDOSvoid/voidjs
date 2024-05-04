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
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_error.h"

namespace voidjs {

using namespace types;
using namespace builtins;

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
GlobalObject* ObjectFactory::NewGlobalObject() {
  auto obj = NewHeapObject(Object::SIZE)->AsGlobalObject();
  obj->SetType(JSType::GLOBAL_OBJECT);
  obj->SetClassType(ObjectClassType::GLOBAL_OBJECT);
  obj->SetPropertyMap(NewPropertyMap());
  obj->SetExtensible(true);
  obj->SetCallable(false);
  obj->SetIsConstructor(false);
  obj->SetPrototype(JSValue::Null());
  return obj;
}

HeapObject* ObjectFactory::NewHeapObject(std::size_t size) {
  auto obj = reinterpret_cast<HeapObject*>(Allocate(HeapObject::SIZE + size));
  obj->SetMetaData(0);
  return obj;
}

RuntimeCallInfo* ObjectFactory::NewRuntimeCallInfo(JSValue this_arg, const std::vector<JSValue>& args) {
  auto len = args.size();
  auto call_info = reinterpret_cast<RuntimeCallInfo*>(
    Allocate(RuntimeCallInfo::SIZE + len * sizeof(JSValue)));
  call_info->SetVM(vm_);
  call_info->SetThis(this_arg);
  call_info->SetArgsNum(len);
  std::copy(args.begin(), args.end(), call_info->GetArgs());
  return call_info;
}

// used to create builtin objects
Object* ObjectFactory::NewEmptyObject(
  std::size_t extra_size, JSType type, ObjectClassType class_type, JSValue proto,
  bool extensible, bool callable, bool is_counstructor) {
}

String* ObjectFactory::NewString(std::u16string_view source) {
  auto len = source.size();
  auto str = NewHeapObject(sizeof(std::size_t) + len * sizeof(char16_t))->AsString();
  str->SetType(JSType::STRING);
  str->SetLength(len);
  std::copy(source.begin(), source.end(), str->GetData());
  return str;
}

String* ObjectFactory::NewStringFromTable(std::u16string_view source) {
  return string_table_->GetOrInsert(source);
}

Object* ObjectFactory::NewObject(
  std::size_t extra_size, JSType type, ObjectClassType class_type,
  JSValue proto, bool extensible, bool callable, bool is_counstructor) {
  auto obj = NewHeapObject(Object::SIZE + extra_size)->AsObject();

  obj->SetType(type);
  obj->SetClassType(class_type);
  obj->SetPropertyMap(NewPropertyMap());
  obj->SetPrototype(proto);
  obj->SetExtensible(extensible);
  obj->SetCallable(callable);
  obj->SetIsConstructor(is_counstructor);
  
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
  const PropertyDescriptor &desc) {
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
  auto prop_map = NewHashMap(PropertyMap::DEFAULT_PROPERTY_NUMS)->AsPropertyMap();
  prop_map->SetType(JSType::PROPERTY_MAP);
  return prop_map;
}

Binding* ObjectFactory::NewBinding(JSValue value, bool _mutable, bool deletable) {
  auto binding = NewHeapObject(Binding::SIZE)->AsBinding();
  binding->SetType(JSType::BINDING);
  binding->SetMutable(_mutable);
  binding->SetDeletable(deletable);
  binding->SetValue(value);
  return binding;
}

InternalFunction* ObjectFactory::NewInternalFunction(InternalFunctionType func) {
  auto internal_func = NewHeapObject(InternalFunction::SIZE)->AsInternalFunction();
  internal_func->SetType(JSType::INTERNAL_FUNCTION);
  internal_func->SetFunction(func);
  internal_func->SetCallable(true);
  return internal_func;
}


HashMap* ObjectFactory::NewHashMap(std::uint32_t capacity) {
  auto hashmap = NewArray(HashMap::HEADER_SIZE + HashMap::ENTRY_SIZE * capacity)->AsHashMap();
  hashmap->SetType(JSType::HASH_MAP);
  hashmap->SetBucketCapacity(capacity);
  hashmap->SetBucketSize(0);
  return hashmap;
}

EnvironmentRecord* ObjectFactory::NewEnvironmentRecord() {
  auto env_rec = NewHeapObject(EnvironmentRecord::SIZE)->AsEnvironmentRecord();
  env_rec->SetType(JSType::ENVIRONMENT_RECORD);
  return env_rec;
}

DeclarativeEnvironmentRecord* ObjectFactory::NewDeclarativeEnvironmentRecord() {
  auto env_rec = NewEnvironmentRecord()->AsDeclarativeEnvironmentRecord();
  env_rec->SetType(JSType::DECLARAVIE_ENVIRONMENT_RECORD);
  env_rec->SetBindingMap(NewHashMap(HashMap::MIN_CAPACITY));
  return env_rec;
}

ObjectEnvironmentRecord* ObjectFactory::NewObjectEnvironmentRecord(Object* obj) {
  auto env_rec = NewEnvironmentRecord()->AsObjectEnvironmentRecord();
  env_rec->SetType((JSType::OBJECT_ENVIRONMENT_RECORD));
  env_rec->SetObject(obj);
  env_rec->SetProvideThis(false);
  return env_rec;
}

LexicalEnvironment* ObjectFactory::NewLexicalEnvironment(LexicalEnvironment* outer, EnvironmentRecord* env_rec) {
  auto env = NewHeapObject(LexicalEnvironment::SIZE)->AsLexicalEnvironment();
  env->SetType(JSType::LEXICAL_ENVIRONMENT);
  env->SetOuter(outer);
  env->SetEnvRec(env_rec);
  return env;
}

JSError* ObjectFactory::NewNativeError(ErrorType type, String* msg) {
  JSValue proto {JSValue::Null()};
  switch (type) {
    case ErrorType::EVAL_ERROR: {
      proto = JSValue{vm_->GetEvalErrorPrototype()};
      break;
    }
    case ErrorType::RANGE_ERROR: {
      proto = JSValue{vm_->GetRangeErrorPrototype()};
      break;
    }
    case ErrorType::REFERENCE_ERROR: {
      proto = JSValue{vm_->GetReferenceErrorPrototype()};
      break;
    }
    case ErrorType::SYNTAX_ERROR: {
      proto = JSValue{vm_->GetSyntaxErrorPrototype()};
      break;
    }
    case ErrorType::TYPE_ERROR: {
      proto = JSValue{vm_->GetTypeErrorPrototype()};
      break;
    }
    case ErrorType::URI_ERROR: {
      proto = JSValue{vm_->GetURIErrorPrototype()};
      break;
    }
    default: {
      proto = JSValue::Null();
      break;
    }
  }

  auto error = NewObject(JSError::SIZE, JSType::JS_ERROR,
                         ObjectClassType::ERROR,
                         proto, true, false, false)->AsJSError();
  return error;
}

JSError* ObjectFactory::NewNativeError(ErrorType type, std::u16string_view msg) {
  return NewNativeError(type, NewStringFromTable(msg));
}

}  // namespace voidjs
