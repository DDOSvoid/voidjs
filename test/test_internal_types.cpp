#include "gtest/gtest.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/hash_map.h"
#include "voidjs/types/lang_types/string.h"

using namespace voidjs;

TEST(InternalTypes, Array) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  std::size_t len = 5;
  auto arr = vm->GetObjectFactory()->NewArray(len);
  
  for (std::size_t idx = 0; idx < len; ++idx) {
    arr->Set(idx, JSValue(static_cast<std::int32_t>(idx)));
  }

  for (std::size_t idx = 0; idx < len; ++idx) {
    EXPECT_TRUE(JSValue::SameValue(JSValue(static_cast<std::int32_t>(idx)),
                                   arr->Get(idx)));
  }

  auto new_arr = types::Array::Append(vm, arr, arr);
  EXPECT_EQ(2 * len, new_arr->GetLength());

  auto expect_arr = std::vector{0, 1, 2, 3, 4, 0, 1, 2, 3, 4};
  for (std::size_t idx = 0; idx < new_arr->GetLength(); ++idx) {
    EXPECT_TRUE(JSValue::SameValue(JSValue(expect_arr[idx]),
                                   new_arr->Get(idx)));
  }
}

TEST(InternalTypes, HashMap) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  auto factory = vm->GetObjectFactory();
  
  auto hashmap = factory->NewHashMap(2);
  
  auto key1 = factory->NewStringFromTable(u"key1");
  auto val1 = JSValue(42);

  auto key2 = factory->NewStringFromTable(u"key2");
  auto val2 = JSValue(factory->NewStringFromTable(u"42"));

  auto key3 = factory->NewStringFromTable(u"key3");
  auto val3 = JSValue(-2);

  auto key4 = factory->NewStringFromTable(u"key4");
  auto val4 = JSValue(100);

  hashmap = types::HashMap::Insert(vm, hashmap, key1, val1);
  hashmap = types::HashMap::Insert(vm, hashmap, key2, val2);
  hashmap = types::HashMap::Insert(vm, hashmap, key3, val3);
  hashmap = types::HashMap::Insert(vm, hashmap, key4, val4);

  {
    auto val = hashmap->Find(vm, key1);
    ASSERT_TRUE(val.IsInt());
    EXPECT_EQ(42, val.GetInt());
  }

  {
    auto val = hashmap->Find(vm, key4);
    ASSERT_TRUE(val.IsInt());
    EXPECT_EQ(100, val.GetInt());
  }
}

TEST(InternalTypes, ProperyMap) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  auto factory = vm->GetObjectFactory();
  
  auto map = factory->NewPropertyMap();

  auto key1 = factory->NewStringFromTable(u"key1");
  auto val1 = types::PropertyDescriptor(JSValue(42));

  auto key2 = factory->NewStringFromTable(u"key2");
  auto val2 = types::PropertyDescriptor(JSValue(2));

  auto key3 = factory->NewStringFromTable(u"key3");
  auto val3 = types::PropertyDescriptor(JSValue(3));

  auto key4 = factory->NewStringFromTable(u"key4");
  auto val4 = types::PropertyDescriptor(JSValue(4));

  map = types::PropertyMap::SetProperty(vm, map, key1, val1);
  map = types::PropertyMap::SetProperty(vm, map, key2, val2);
  map = types::PropertyMap::SetProperty(vm, map, key3, val3);
  map = types::PropertyMap::SetProperty(vm, map, key4, val4);

  auto val = map->GetProperty(key4);
  ASSERT_TRUE(val.IsHeapObject() && val.GetHeapObject()->IsDataPropertyDescriptor());
  EXPECT_EQ(val4.GetValue().GetInt(), val.GetHeapObject()->AsDataPropertyDescriptor()->GetValue().GetInt());

  map->DeleteProperty(key1);

  auto emp_val = map->GetProperty(key1);
  EXPECT_TRUE(emp_val.IsEmpty());
}
