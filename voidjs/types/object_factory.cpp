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
  obj->SetProperties(JSValue(NewPropertyMap()));
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

RuntimeCallInfo* ObjectFactory::NewRuntimeCallInfo(std::size_t args_num) {
  auto call_info = reinterpret_cast<RuntimeCallInfo*>(
    Allocate(sizeof(std::uint64_t) + args_num * sizeof(JSValue)));
  call_info->SetArgsNum(args_num);
  std::fill_n(call_info->GetArgs(), args_num, JSValue{});
  return call_info;
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

Object* ObjectFactory::NewObject(JSValue proto) {
  auto obj = NewHeapObject(Object::SIZE)->AsObject();

  obj->SetType(JSType::OBJECT);
  obj->SetProperties(JSValue(NewPropertyMap()));
  obj->SetPrototype(proto);
  
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

// used to create builtin object
Object* ObjectFactory::NewEmptyObject(std::size_t extra_size) {
  auto obj = NewHeapObject(Object::SIZE + extra_size)->AsObject();
  return obj;
}

// new Object ( [ value ] )
// Defined in ECMAScript 5.1 Chapter 15.2.2.1
JSObject* ObjectFactory::NewJSObject(JSValue value) {
  // 1. If value is supplied, then
  if (!value.IsEmpty()) {
    // a .If Type(value) is Object, then
    if (value.IsObject()) {
      // i. If the value is a native ECMAScript object, do not create a new object but simply return value.
      if (value.GetHeapObject()->IsJSObject()) {
        return value.GetHeapObject()->AsJSObject();
      }
      
      // ii. If the value is a host object, then actions are taken and
      //     a result is returned in an implementation-dependent manner that may depend on the host object.
      // todo
    }
    
    // b. If Type(value) is String, return ToObject(value).
    // c. If Type(value) is Boolean, return ToObject(value).
    // d. If Type(value) is Number, return ToObject(value).
    if (value.IsString() || value.IsBoolean() || value.IsNumber()) {
      return JSValue::ToObject(vm_, value)->AsJSObject();
    }
  }

  // 2. Assert: The argument value was not supplied or its type was Null or Undefined.
  
  // 3. Let obj be a newly created native ECMAScript object.
  // 4. Set the [[Prototype]] internal property of obj t to the standard built-in Object prototype object (15.2.4).
  // todo
  auto obj = NewObject(JSValue(vm_->GetObjectPrototype()))->AsJSObject();
  obj->SetType(JSType::JS_OBJECT);
  
  // 5 .Set the [[Class]] internal property of obj to "Object".
  obj->SetClassType(ObjectClassType::OBJECT);
  
  // 6. Set the [[Extensible]] internal property of obj to true.
  obj->SetExtensible(true);
  
  // 7. Set the all the internal methods of obj as specified in 8.12
  
  // 8. Return obj.
  return obj;
}

JSError* ObjectFactory::NewNativeError(ErrorType type, String* msg) {
  JSValue proto {JSValue::Null()};
  switch (type) {
    case ErrorType::EVAL_ERROR: {
      proto = JSValue(vm_->GetEvalErrorPrototype());
      break;
    }
    case ErrorType::RANGE_ERROR: {
      proto = JSValue(vm_->GetRangeErrorPrototype());
      break;
    }
    case ErrorType::REFERENCE_ERROR: {
      proto = JSValue(vm_->GetReferenceErrorPrototype());
      break;
    }
    case ErrorType::SYNTAX_ERROR: {
      proto = JSValue(vm_->GetSyntaxErrorPrototype());
      break;
    }
    case ErrorType::TYPE_ERROR: {
      proto = JSValue(vm_->GetTypeErrorPrototype());
      break;
    }
    case ErrorType::URI_ERROR: {
      proto = JSValue(vm_->GetURIErrorPrototype());
      break;
    }
    default: {
      proto = JSValue::Null();
      break;
    }
  }

  auto error = NewObject(proto)->AsJSError();
  
  error->SetClassType(ObjectClassType::ERROR);
  error->SetExtensible(true);

  return error;
}

JSError* ObjectFactory::NewNativeError(ErrorType type, std::u16string_view msg) {
  return NewNativeError(type, NewStringFromTable(msg));
}

}  // namespace voidjs
