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
  
  auto key1 = factory->NewString(u"key1");
  auto val1 = JSHandle<JSValue>{vm, JSValue{42}};

  auto key2 = factory->NewString(u"key2");
  auto val2 = factory->NewString(u"42").As<JSValue>();

  auto key3 = factory->NewString(u"key3");
  auto val3 = JSHandle<JSValue>{vm, JSValue{-2}};

  auto key4 = factory->NewString(u"key4");
  auto val4 = JSHandle<JSValue>{vm, JSValue{100}};

  auto key5 = factory->NewString(u"1");
  auto val5 = JSHandle<JSValue>{vm, JSValue{0}};

  auto key6 = factory->NewString(u"2");
  auto val6 = JSHandle<JSValue>{vm, JSValue{0}};

  auto key7 = factory->NewString(u"3");
  auto val7 = JSHandle<JSValue>{vm, JSValue{-10}};

  auto key8 = factory->NewString(u"4");
  auto val8 = JSHandle<JSValue>{vm, JSValue{0}};

  hashmap = types::HashMap::Insert(vm, hashmap, key1, val1);
  hashmap = types::HashMap::Insert(vm, hashmap, key2, val2);
  hashmap = types::HashMap::Insert(vm, hashmap, key3, val3);
  hashmap = types::HashMap::Insert(vm, hashmap, key4, val4);
  hashmap = types::HashMap::Insert(vm, hashmap, key5, val5);
  hashmap = types::HashMap::Insert(vm, hashmap, key6, val6);
  hashmap = types::HashMap::Insert(vm, hashmap, key7, val7);
  hashmap = types::HashMap::Insert(vm, hashmap, key8, val8);

  {
    auto val = hashmap->Find(vm, key1);
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(42, val->GetInt());
  }

  {
    auto val = hashmap->Find(vm, key4);
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(100, val->GetInt());
  }

  hashmap->Erase(vm, key2);
  
  {
    auto val = hashmap->Find(vm, key2);
    EXPECT_TRUE(val.IsEmpty());
  }

  {
    auto val = hashmap->Find(vm, key7);
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(-10, val->GetInt());
  }
}

TEST(InternalTypes, PropertyMap) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  auto factory = vm->GetObjectFactory();
  
  auto map = factory->NewPropertyMap();

  auto key1 = factory->NewString(u"key1");
  auto val1 = types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, JSValue{42}}};

  auto key2 = factory->NewString(u"key2");
  auto val2 = types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, JSValue{2}}};

  auto key3 = factory->NewString(u"key3");
  auto val3 = types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, JSValue{3}}};

  auto key4 = factory->NewString(u"key4");
  auto val4 = types::PropertyDescriptor{vm, JSHandle<JSValue>{vm, JSValue{4}}};

  map = types::PropertyMap::SetProperty(vm, map, key1, val1);
  map = types::PropertyMap::SetProperty(vm, map, key2, val2);
  map = types::PropertyMap::SetProperty(vm, map, key3, val3);
  map = types::PropertyMap::SetProperty(vm, map, key4, val4);

  auto val = map->GetProperty(vm, key4);
  ASSERT_TRUE(val->IsHeapObject() && val->GetHeapObject()->IsDataPropertyDescriptor());
  EXPECT_EQ(val4.GetValue()->GetInt(), val->GetHeapObject()->AsDataPropertyDescriptor()->GetValue().GetInt());

  map->DeleteProperty(vm, key1);

  auto emp_val = map->GetProperty(vm, key1);
  EXPECT_TRUE(emp_val.IsEmpty());
}
