#include "gtest/gtest.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/lang_types/string.h"

using namespace voidjs;

TEST(InternalTypes, Array) {
  std::size_t len = 5;
  
  auto arr = ObjectFactory::NewArray(len);

  
  for (std::size_t idx = 0; idx < len; ++idx) {
    arr->Set(idx, JSValue(static_cast<std::int32_t>(idx)));
  }

  for (std::size_t idx = 0; idx < len; ++idx) {
    EXPECT_TRUE(JSValue::SameValue(JSValue(static_cast<std::int32_t>(idx)),
                                   arr->Get(idx)));
  }

  auto new_arr = types::Array::Append(arr, arr);
  EXPECT_EQ(2 * len, new_arr->GetLength());

  auto expect_arr = std::vector{0, 1, 2, 3, 4, 0, 1, 2, 3, 4};
  for (std::size_t idx = 0; idx < new_arr->GetLength(); ++idx) {
    EXPECT_TRUE(JSValue::SameValue(JSValue(expect_arr[idx]),
                                   new_arr->Get(idx)));
  }
}

TEST(InternalTypes, ProperyMap) {
  auto map = ObjectFactory::NewPropertyMap();

  auto key1 = JSValue(ObjectFactory::NewString(u"key1"));
  auto val1 = types::PropertyDescriptor(JSValue(42));

  auto key2 = JSValue(ObjectFactory::NewString(u"key2"));
  auto val2 = types::PropertyDescriptor(JSValue(2));

  auto key3 = JSValue(ObjectFactory::NewString(u"key3"));
  auto val3 = types::PropertyDescriptor(JSValue(3));

  auto key4 = JSValue(ObjectFactory::NewString(u"key4"));
  auto val4 = types::PropertyDescriptor(JSValue(4));

  map = types::PropertyMap::SetProperty(map, key1, val1);
  map = types::PropertyMap::SetProperty(map, key2, val2);
  map = types::PropertyMap::SetProperty(map, key3, val3);
  map = types::PropertyMap::SetProperty(map, key4, val4);

  auto val = map->GetProperty(key4);
  ASSERT_TRUE(val.IsHeapObject() && val.GetHeapObject()->IsDataPropertyDescriptor());
  EXPECT_EQ(val4.GetValue().GetInt(), val.GetHeapObject()->AsDataPropertyDescriptor()->GetValue().GetInt());

  map->DeleteProperty(key1);

  auto emp_val = map->GetProperty(key1);
  EXPECT_TRUE(emp_val.IsEmpty());
}
