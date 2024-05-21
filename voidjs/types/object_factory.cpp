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
#include "voidjs/interpreter/string_table.h"

namespace voidjs {

ObjectFactory::~ObjectFactory() {
  delete heap_;
  
  delete string_table_;
}

// used to create builtin objects
JSHandle<types::Object> ObjectFactory::NewEmptyObject(
  std::size_t extra_size, JSType type, ObjectClassType class_type, JSValue proto,
  bool extensible, bool callable, bool is_counstructor) {
}


JSHandle<types::String> ObjectFactory::NewStringFromInt(std::int32_t i) {
  return JSValue::NumberToString(vm_, i);
}

JSHandle<types::String> ObjectFactory::NewString(char16_t ch) {
  return NewString(std::u16string{1, ch});
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
  auto internal_func = NewObject(types::InternalFunction::SIZE, JSType::INTERNAL_FUNCTION, ObjectClassType::FUNCTION,
                                 vm_->GetFunctionPrototype().As<JSValue>(), true, true, false).As<types::InternalFunction>();
  internal_func->SetFunction(func);
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
  return NewNativeError(type, NewString(msg));
}

}  // namespace voidjs
