#ifndef VOIDJS_TYPES_HEAP_OBJECT_H
#define VOIDJS_TYPES_HEAP_OBJECT_H

#include <cstddef>
#include <cstdint>

#include "voidjs/utils/helper.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/js_type.h"

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

}  // namespace types

class HeapObject {
 public:
  // meta data
  
  // JSType type             8 bits
  
  // bool extensible         1 bit
  // bool callable           1 bit
  // bool is_constructor     1 bit

  // bool writable           1 bit
  // bool enumerable         1 bit
  // bool configurable       1 bit

  // bool deletable          1 bit
  // bool mutable            1 bit
  
  static constexpr std::size_t META_DATA_OFFSET = 0;
  static constexpr std::size_t META_DATA_SIZE = sizeof(std::uint64_t);
  std::uint64_t* GetMetaData() const { return utils::BitGet<std::uint64_t*>(this, META_DATA_OFFSET); }
  void SetMetaData(std::uint64_t data) const { *utils::BitGet<std::uint64_t*>(this, META_DATA_OFFSET) = data; }
  
  using TypeBitSet = utils::BitSet<JSType, 0, 8>;
  JSType GetType() const { return TypeBitSet::Get(*GetMetaData()); }
  void SetType(JSType type) { TypeBitSet::Set(GetMetaData(), type); }

  using ExtensibleBitSet = utils::BitSet<bool, 8, 9>;
  bool GetExtensible() const { return ExtensibleBitSet::Get(*GetMetaData()); }
  void SetExtensible(bool flag) { ExtensibleBitSet::Set(GetMetaData(), flag); }

  using CallableBitSet = utils::BitSet<bool, 9, 10>;
  bool GetCallable() const { return CallableBitSet::Get(*GetMetaData()); }
  void SetCallable(bool flag) { CallableBitSet::Set(GetMetaData(), flag); }

  using IsConstructorBitSet = utils::BitSet<bool, 10, 11>;
  bool IsConstructor() const { return IsConstructorBitSet::Get(*GetMetaData()); }
  void SetIsConstructor(bool flag) { IsConstructorBitSet::Set(GetMetaData(), flag); }
  
  using WritableBitSet = utils::BitSet<bool, 11, 12>;
  bool GetWritable() const { return WritableBitSet::Get(*GetMetaData()); }
  void SetWritable(bool flag) { WritableBitSet::Set(GetMetaData(), flag); }

  using EnumerableBitSet = utils::BitSet<bool, 12, 13>;
  bool GetEnumerable() const { return EnumerableBitSet::Get(*GetMetaData()); }
  void SetEnumerable(bool flag) { EnumerableBitSet::Set(GetMetaData(), flag); }

  using ConfigurableBitSet = utils::BitSet<bool, 13, 14>;
  bool GetConfigurable() const { return ConfigurableBitSet::Get(*GetMetaData()); }
  void SetConfigurable(bool flag) { ConfigurableBitSet::Set(GetMetaData(), flag); }

  using DeletableBitSet = utils::BitSet<bool, 14, 15>;
  bool GetDeletable() const { return DeletableBitSet::Get(*GetMetaData()); }
  void SetDeletable(bool flag) { DeletableBitSet::Set(GetMetaData(), flag); }

  using MutableBitSet = utils::BitSet<bool, 15, 16>;
  bool GetMutable() const { return MutableBitSet::Get(*GetMetaData()); }
  void SetMutable(bool flag) { MutableBitSet::Set(GetMetaData(), flag); }

  static constexpr std::size_t SIZE = META_DATA_SIZE;
  static constexpr std::size_t OFFSET = META_DATA_OFFSET + META_DATA_SIZE;
 
  
  // Is Check
  bool IsString() const { return GetType() == JSType::STRING; }
  bool IsObject() const { return GetType() == JSType::OBJECT; }
  bool IsArray() const { return GetType() == JSType::ARRAY; }
  bool IsDataPropertyDescriptor() const { return GetType() == JSType::DATA_PROPERTY_DESCRIPTOR; }
  bool IsAccessorPropertyDescriptor() const { return GetType() == JSType::ACCESSOR_PROPERTY_DESCRIPTOR; }
  bool IsGenericPropertyDescriptor() const { return GetType() == JSType::GENERIC_PROPERTY_DESCRIPTOR; }
  bool IsPropertyMap() const { return GetType() == JSType::PROPERTY_MAP; }
  bool IsBindgin() const { return GetType() == JSType::BINDING; }

  // As Cast
  types::String* AsString() { return reinterpret_cast<types::String*>(this); }
  types::Object* AsObject() { return reinterpret_cast<types::Object*>(this); }
  types::Array* AsArray() { return reinterpret_cast<types::Array*>(this); }
  types::DataPropertyDescriptor* AsDataPropertyDescriptor() { return reinterpret_cast<types::DataPropertyDescriptor*>(this); }
  types::AccessorPropertyDescriptor* AsAccessorPropertyDescriptor() { return reinterpret_cast<types::AccessorPropertyDescriptor*>(this); }
  types::GenericPropertyDescriptor* AsGenericPropertyDescriptor() { return reinterpret_cast<types::GenericPropertyDescriptor*>(this); }
  types::PropertyMap* AsPropertyMap() { return reinterpret_cast<types::PropertyMap*>(this); }
  types::Binding* AsBinding() { return reinterpret_cast<types::Binding*>(this); }

  // As Cast
  const types::String* AsString() const { return reinterpret_cast<const types::String*>(this); }
  const types::Object* AsObject() const { return reinterpret_cast<const types::Object*>(this); }
  const types::Array* AsArray() const { return reinterpret_cast<const types::Array*>(this); }
  const types::DataPropertyDescriptor* AsDataPropertyDescriptor() const { return reinterpret_cast<const types::DataPropertyDescriptor*>(this); }
  const types::AccessorPropertyDescriptor* AsAccessorPropertyDescriptor() const { return reinterpret_cast<const types::AccessorPropertyDescriptor*>(this); }
  const types::GenericPropertyDescriptor* AsGenericPropertyDescriptor() const { return reinterpret_cast<const types::GenericPropertyDescriptor*>(this); }
  const types::PropertyMap* AsPropertyMap() const { return reinterpret_cast<const types::PropertyMap*>(this); }
  const types::Binding* AsBinding() const { return reinterpret_cast<const types::Binding*>(this); }
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_HEAP_OBJECT_H
