#include "gtest/gtest.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/js_value.h"

using namespace voidjs;

TEST(InternalTypes, Array) {
  std::size_t len = 5;
  
  auto arr = types::Array::New(len);

  
  for (std::size_t idx = 0; idx < len; ++idx) {
    arr->SetByIndex(idx, JSValue(static_cast<std::int32_t>(idx)));
  }

  for (std::size_t idx = 0; idx < len; ++idx) {
    EXPECT_TRUE(JSValue::SameValue(JSValue(static_cast<std::int32_t>(idx)),
                                   arr->GetByIndex(idx)));
  }

  auto new_arr = types::Array::Append(JSValue(arr), JSValue(arr)).GetHeapObject()->AsArray();
  EXPECT_EQ(2 * len, new_arr->GetLength());

  auto expect_arr = std::vector{0, 1, 2, 3, 4, 0, 1, 2, 3, 4};
  for (std::size_t idx = 0; idx < new_arr->GetLength(); ++idx) {
    EXPECT_TRUE(JSValue::SameValue(JSValue(expect_arr[idx]),
                                   new_arr->GetByIndex(idx)));
  }
}
