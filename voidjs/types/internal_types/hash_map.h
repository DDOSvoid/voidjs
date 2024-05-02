#ifndef VOIDJS_TYPES_INTERNAL_TYPE_HASH_MAP_H
#define VOIDJS_TYPES_INTERNAL_TYPE_HASH_MAP_H

#include <functional>

#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {
namespace types {

class HashMap : public Array {
 public:
  static constexpr std::uint32_t BUCKET_SIZE_INDEX = 0;
  std::int32_t GetBucketSize() const { return Get(BUCKET_SIZE_INDEX).GetInt(); }
  void SetBucketSize(std::int32_t size) { Set(BUCKET_SIZE_INDEX, JSValue(size)); }
  void IncreaseBucketSize() { SetBucketSize(GetBucketSize() + 1); }
  void DecreaseBucketSize() { SetBucketSize(GetBucketSize() - 1); }
  
  static constexpr std::uint32_t BUCKET_CAPACITY_INDEX = 1;
  std::int32_t GetBucketCapacity() const { return Get(BUCKET_CAPACITY_INDEX).GetInt(); }
  void SetBucketCapacity(std::int32_t capacity) { return Set(BUCKET_CAPACITY_INDEX, JSValue(capacity)); }

  using Hash = utils::detail::hash<std::u16string_view>;
  static constexpr std::uint32_t MIN_CAPACITY = 2;

  static constexpr std::uint32_t HEADER_SIZE       = 2;
  static constexpr std::uint32_t ENTRY_SIZE        = 2;
  static constexpr std::uint32_t ENTRY_KEY_INDEX   = 0;
  static constexpr std::uint32_t ENTRY_VALUE_INEDX = 1;
  JSValue GetKey(std::uint32_t entry) const {
    return Get(HEADER_SIZE + entry * ENTRY_SIZE + ENTRY_KEY_INDEX);
  }
  void SetKey(std::uint32_t entry, JSValue key) {
    Set(HEADER_SIZE + entry * ENTRY_SIZE + ENTRY_KEY_INDEX, key);
  }
  JSValue GetValue(std::uint32_t entry) const {
    return Get(HEADER_SIZE + entry * ENTRY_SIZE + ENTRY_VALUE_INEDX);
  }
  void SetValue(std::uint32_t entry, JSValue value) {
    Set(HEADER_SIZE + entry * ENTRY_SIZE + ENTRY_VALUE_INEDX, value);
  }
  

  static HashMap* Insert(VM* vm, HashMap* hashmap, String* key, JSValue value) {
    auto new_hashmap = std::invoke([=]() {
      if (!hashmap->IsFull()) {
        return hashmap;
      }
      
      auto new_hashmap = Reserve(vm, hashmap, hashmap->GetBucketCapacity() << 1);
      return new_hashmap;
    });
    
    auto entry = new_hashmap->FindEntry(key);

    if (new_hashmap->GetKey(entry).IsEmpty()) {
      new_hashmap->AddEntry(entry, key, value);
    } else {
      new_hashmap->SetValue(entry, value);
    }

    return new_hashmap;
  }

  void Erase(VM* vm, String* key) {
    auto entry = FindEntry(key);

    if (!GetKey(entry).IsEmpty()) {
      DeleteEntry(entry);
    }
  }

  JSValue Find(VM* vm, String* key) const {
    auto entry = FindEntry(key);

    if (!GetKey(entry).IsEmpty()) {
      return GetValue(entry);
    } else {
      return {};
    }
  }

 private:
  static HashMap* Reserve(VM* vm, HashMap* hashmap, std::uint32_t capacity) {
    if (capacity <= hashmap->GetBucketCapacity()) {
      return hashmap;
    }

    auto new_hashmap = vm->GetObjectFactory()->NewHashMap(capacity);
    Rehash(new_hashmap, hashmap);
    
    return new_hashmap;
  }

  static void Rehash(HashMap* new_hashmap, HashMap* old_hashmap) {
    auto old_capacity = old_hashmap->GetBucketCapacity();
    for (std::uint32_t idx = 0; idx < old_capacity; ++idx) {
      auto key = old_hashmap->GetKey(idx);
      if (key.IsEmpty()) {
        continue;
      }

      auto value = old_hashmap->GetValue(idx);
      auto entry = new_hashmap->FindEntry(key.GetHeapObject()->AsString());
      new_hashmap->AddEntry(entry, key.GetHeapObject()->AsString(), value);
    }
  }
  
  std::uint32_t FindEntry(String* key) const {
    auto hash = Hash{}(key->GetString());
    for (std::uint32_t entry = GetFirstPosition(hash), cnt = 0; ; entry = GetNextPosition(entry, ++cnt)) {
      auto key_val = GetKey(entry);
      if (key_val.IsEmpty()) {
        return entry;
      }
      // Else key must be String*
      if (key_val.GetHeapObject()->AsString()->Equal(key)) {
        return entry;
      }
    }

    // unreached branch
    // todo
  }

  std::uint32_t GetFirstPosition(std::uint64_t hash) const {
    return hash & GetBucketCapacity() - 1;
  }

  std::uint32_t GetNextPosition(std::uint32_t last_pos, std::uint32_t cnt) const {
    return (last_pos + cnt * (cnt + 1) / 2) & GetBucketCapacity() - 1;
  }

  void AddEntry(std::uint32_t entry, String* key, JSValue value) {
    SetKey(entry, JSValue(key));
    SetValue(entry, value);
    IncreaseBucketSize();
  }

  void DeleteEntry(std::uint32_t entry) {
    SetKey(entry, JSValue{});
    SetValue(entry, JSValue{});
    DecreaseBucketSize();
  }

  bool IsFull() {
    return 1.0 * GetBucketSize() / GetBucketCapacity() >= 0.7;
  }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_INTERNAL_TYPE_HASH_MAP_H