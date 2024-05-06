#ifndef VOIDJS_TYPES_HEAP_OBJECT_H
#define VOIDJS_TYPES_HEAP_OBJECT_H

#include <cstddef>
#include <cstdint>

#include "voidjs/utils/helper.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/error_type.h"

namespace voidjs {
namespace types {

class String;
class Object;
class Array;
class DataPropertyDescriptor;
class AccessorPropertyDescriptor;
class GenericPropertyDescriptor;
class PropertyMap;
class Binding;
class InternalFunction;
class HashMap;
class EnvironmentRecord;
class DeclarativeEnvironmentRecord;
class ObjectEnvironmentRecord;
class LexicalEnvironment;

}  // namespace types

namespace builtins {

class GlobalObject;
class JSObject;
class JSFunction;
class JSArray;
class JSString;
class JSBoolean;
class JSNumber;
class JSError;

}  // namespace builtins

class HeapObject {
 public:
  // meta data                         64 bits
  
  // enum JSType type                  8 bits
  
  // enum ObjectClassType class_type   8 bits

  // enum ErrorType error_type         8 bits

  // Object internal properties
  // bool extensible                   1 bit
  // bool callable                     1 bit
  // bool is_constructor               1 bit

  // Property attributes
  // bool writable                     1 bit
  // bool enumerable                   1 bit
  // bool configurable                 1 bit

  // Binding attributes
  // bool deletable                    1 bit
  // bool mutable                      1 bit

  // ObjectEnvironmentRecord properies
  // bool provide_this                 1 bit
  
  static constexpr std::size_t META_DATA_OFFSET = 0;
  static constexpr std::size_t META_DATA_SIZE = sizeof(std::uint64_t);
  std::uint64_t* GetMetaData() const { return utils::BitGet<std::uint64_t*>(this, META_DATA_OFFSET); }
  void SetMetaData(std::uint64_t data) const { *utils::BitGet<std::uint64_t*>(this, META_DATA_OFFSET) = data; }
  
  using TypeBitSet = utils::BitSet<JSType, 0, 8>;
  JSType GetType() const { return TypeBitSet::Get(*GetMetaData()); }
  void SetType(JSType type) { TypeBitSet::Set(GetMetaData(), type); }

  using ClassTypeBitSet = utils::BitSet<ObjectClassType, 8, 16>;
  ObjectClassType GetClassType() const { return ClassTypeBitSet::Get(*GetMetaData()); }
  void SetClassType(ObjectClassType type) { ClassTypeBitSet::Set(GetMetaData(), type); }

  using ErrorTypeBitSet = utils::BitSet<ErrorType, 16, 24>;
  ErrorType GetErrorType() const { return ErrorTypeBitSet::Get(*GetMetaData()); }
  void SetErrorType(ErrorType type) { ErrorTypeBitSet::Set(GetMetaData(), type); }

  using ExtensibleBitSet = utils::BitSet<bool, 24, 25>;
  bool GetExtensible() const { return ExtensibleBitSet::Get(*GetMetaData()); }
  void SetExtensible(bool flag) { ExtensibleBitSet::Set(GetMetaData(), flag); }

  using CallableBitSet = utils::BitSet<bool, 25, 26>;
  bool GetCallable() const { return CallableBitSet::Get(*GetMetaData()); }
  void SetCallable(bool flag) { CallableBitSet::Set(GetMetaData(), flag); }

  using IsConstructorBitSet = utils::BitSet<bool, 26, 27>;
  bool IsConstructor() const { return IsConstructorBitSet::Get(*GetMetaData()); }
  void SetIsConstructor(bool flag) { IsConstructorBitSet::Set(GetMetaData(), flag); }
  
  using WritableBitSet = utils::BitSet<bool, 27, 28>;
  bool GetWritable() const { return WritableBitSet::Get(*GetMetaData()); }
  void SetWritable(bool flag) { WritableBitSet::Set(GetMetaData(), flag); }

  using EnumerableBitSet = utils::BitSet<bool, 28, 29>;
  bool GetEnumerable() const { return EnumerableBitSet::Get(*GetMetaData()); }
  void SetEnumerable(bool flag) { EnumerableBitSet::Set(GetMetaData(), flag); }

  using ConfigurableBitSet = utils::BitSet<bool, 29, 30>;
  bool GetConfigurable() const { return ConfigurableBitSet::Get(*GetMetaData()); }
  void SetConfigurable(bool flag) { ConfigurableBitSet::Set(GetMetaData(), flag); }

  using DeletableBitSet = utils::BitSet<bool, 30, 31>;
  bool GetDeletable() const { return DeletableBitSet::Get(*GetMetaData()); }
  void SetDeletable(bool flag) { DeletableBitSet::Set(GetMetaData(), flag); }

  using MutableBitSet = utils::BitSet<bool, 31, 32>;
  bool GetMutable() const { return MutableBitSet::Get(*GetMetaData()); }
  void SetMutable(bool flag) { MutableBitSet::Set(GetMetaData(), flag); }

  using ProvideThisBitSet = utils::BitSet<bool, 32, 33>;
  bool GetProvideThis() const { return ProvideThisBitSet::Get(*GetMetaData()); }
  void SetProvideThis(bool flag) { ProvideThisBitSet::Set(GetMetaData(), flag); }

  static constexpr std::size_t SIZE = META_DATA_SIZE;
  static constexpr std::size_t END_OFFSET = META_DATA_OFFSET + META_DATA_SIZE;
 
  
  // Is Check
  bool IsString() const { return GetType() == JSType::STRING; }
  bool IsObject() const { return GetType() == JSType::OBJECT; }
  bool IsArray() const { return GetType() == JSType::ARRAY; }
  bool IsDataPropertyDescriptor() const { return GetType() == JSType::DATA_PROPERTY_DESCRIPTOR; }
  bool IsAccessorPropertyDescriptor() const { return GetType() == JSType::ACCESSOR_PROPERTY_DESCRIPTOR; }
  bool IsGenericPropertyDescriptor() const { return GetType() == JSType::GENERIC_PROPERTY_DESCRIPTOR; }
  bool IsPropertyMap() const { return GetType() == JSType::PROPERTY_MAP; }
  bool IsBindgin() const { return GetType() == JSType::BINDING; }
  bool IsInternalFunction() const { return GetType() == JSType::INTERNAL_FUNCTION; }
  bool IsHashMap() const { return GetType() == JSType::HASH_MAP; }
  bool IsEnvironmentRecord() const { return GetType() == JSType::ENVIRONMENT_RECORD; }
  bool IsDeclarativeEnvironmentRecord() const { return GetType() == JSType::DECLARAVIE_ENVIRONMENT_RECORD; }
  bool IsObjectEnvironmentRecord() const { return GetType() == JSType::OBJECT_ENVIRONMENT_RECORD; }
  bool IsLexicalEnvironment() const { return GetType() == JSType::LEXICAL_ENVIRONMENT; }
  bool IsGlobalObject() const { return GetType() == JSType::GLOBAL_OBJECT; }
  bool IsJSObject() const { return GetType() == JSType::JS_OBJECT; }
  bool IsJSFunction() const { return GetType() == JSType::JS_FUNCTION; }
  bool IsJSArray() const { return GetType() == JSType::JS_ARRAY; }
  bool IsJSString() const { return GetType() == JSType::JS_STRING; }
  bool IsJSBoolean() const { return GetType() == JSType::JS_BOOLEAN; }
  bool IsJSNumber() const { return GetType() == JSType::JS_NUMBER; }
  bool IsJSError() const { return GetType() == JSType::JS_ERROR; }

  template <typename T>
  T* As() { return reinterpret_cast<T*>(this); }
  template <typename T>
  const T* As() const { return reinterpret_cast<const T*>(this); }

  // As Cast
  types::String* AsString() { return reinterpret_cast<types::String*>(this); }
  types::Object* AsObject() { return reinterpret_cast<types::Object*>(this); }
  types::Array* AsArray() { return reinterpret_cast<types::Array*>(this); }
  types::DataPropertyDescriptor* AsDataPropertyDescriptor() { return reinterpret_cast<types::DataPropertyDescriptor*>(this); }
  types::AccessorPropertyDescriptor* AsAccessorPropertyDescriptor() { return reinterpret_cast<types::AccessorPropertyDescriptor*>(this); }
  types::GenericPropertyDescriptor* AsGenericPropertyDescriptor() { return reinterpret_cast<types::GenericPropertyDescriptor*>(this); }
  types::PropertyMap* AsPropertyMap() { return reinterpret_cast<types::PropertyMap*>(this); }
  types::Binding* AsBinding() { return reinterpret_cast<types::Binding*>(this); }
  types::InternalFunction* AsInternalFunction() { return reinterpret_cast<types::InternalFunction*>(this); }
  types::HashMap* AsHashMap() { return reinterpret_cast<types::HashMap*>(this); }
  types::EnvironmentRecord* AsEnvironmentRecord() { return reinterpret_cast<types::EnvironmentRecord*>(this); }
  types::DeclarativeEnvironmentRecord* AsDeclarativeEnvironmentRecord() { return reinterpret_cast<types::DeclarativeEnvironmentRecord*>(this); }
  types::ObjectEnvironmentRecord* AsObjectEnvironmentRecord() { return reinterpret_cast<types::ObjectEnvironmentRecord*>(this); }
  types::LexicalEnvironment* AsLexicalEnvironment() { return reinterpret_cast<types::LexicalEnvironment*>(this); }
  builtins::GlobalObject* AsGlobalObject() { return reinterpret_cast<builtins::GlobalObject*>(this); }
  builtins::JSObject* AsJSObject() { return reinterpret_cast<builtins::JSObject*>(this); }
  builtins::JSFunction* AsJSFunction() { return reinterpret_cast<builtins::JSFunction*>(this); }
  builtins::JSArray* AsJSArray() { return reinterpret_cast<builtins::JSArray*>(this); }
  builtins::JSString* AsJSString() { return reinterpret_cast<builtins::JSString*>(this); }
  builtins::JSBoolean* AsJSBoolean() { return reinterpret_cast<builtins::JSBoolean*>(this); }
  builtins::JSNumber* AsJSNumber() { return reinterpret_cast<builtins::JSNumber*>(this); }
  builtins::JSError* AsJSError() { return reinterpret_cast<builtins::JSError*>(this); }

  // As Cast
  const types::String* AsString() const { return reinterpret_cast<const types::String*>(this); }
  const types::Object* AsObject() const { return reinterpret_cast<const types::Object*>(this); }
  const types::Array* AsArray() const { return reinterpret_cast<const types::Array*>(this); }
  const types::DataPropertyDescriptor* AsDataPropertyDescriptor() const { return reinterpret_cast<const types::DataPropertyDescriptor*>(this); }
  const types::AccessorPropertyDescriptor* AsAccessorPropertyDescriptor() const { return reinterpret_cast<const types::AccessorPropertyDescriptor*>(this); }
  const types::GenericPropertyDescriptor* AsGenericPropertyDescriptor() const { return reinterpret_cast<const types::GenericPropertyDescriptor*>(this); }
  const types::PropertyMap* AsPropertyMap() const { return reinterpret_cast<const types::PropertyMap*>(this); }
  const types::Binding* AsBinding() const { return reinterpret_cast<const types::Binding*>(this); }
  const types::InternalFunction* AsInternalFunction() const { return reinterpret_cast<const types::InternalFunction*>(this); }
  const types::HashMap* AsHashMap() const { return reinterpret_cast<const types::HashMap*>(this); }
  const types::EnvironmentRecord* AsEnvironmentRecord() const { return reinterpret_cast<const types::EnvironmentRecord*>(this); }
  const types::DeclarativeEnvironmentRecord* AsDeclarativeEnvironmentRecord() const { return reinterpret_cast<const types::DeclarativeEnvironmentRecord*>(this); }
  const types::ObjectEnvironmentRecord* AsObjectEnvironmentRecord() const { return reinterpret_cast<const types::ObjectEnvironmentRecord*>(this); }
  const types::LexicalEnvironment* AsLexicalEnvironment() const { return reinterpret_cast<const types::LexicalEnvironment*>(this); }
  const builtins::GlobalObject* AsGlobalObject() const { return reinterpret_cast<const builtins::GlobalObject*>(this); }
  const builtins::JSObject* AsJSObject() const { return reinterpret_cast<const builtins::JSObject*>(this); }
  const builtins::JSFunction* AsJSFunction() const { return reinterpret_cast<const builtins::JSFunction*>(this); }
  const builtins::JSArray* AsJSArray() const { return reinterpret_cast<const builtins::JSArray*>(this); }
  const builtins::JSString* AsJSString() const { return reinterpret_cast<const builtins::JSString*>(this); }
  const builtins::JSBoolean* AsJSBoolean() const { return reinterpret_cast<const builtins::JSBoolean*>(this); }
  const builtins::JSNumber* AsJSNumber() const { return reinterpret_cast<const builtins::JSNumber*>(this); }
  const builtins::JSError* AsJSError() const { return reinterpret_cast<const builtins::JSError*>(this); }
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_HEAP_OBJECT_H
