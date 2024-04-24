#include "gtest/gtest.h"

#include <limits>
#include <cmath>

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"

using namespace voidjs;

TEST(JSValue, ToNumber) {
  auto nan = std::numeric_limits<double>::quiet_NaN();
  auto inf = std::numeric_limits<double>::infinity();
  
  EXPECT_EQ(42, JSValue::ToNumber(JSValue(42)).GetInt());
  EXPECT_DOUBLE_EQ(0.01, JSValue::ToNumber(JSValue(0.01)).GetDouble());
  EXPECT_TRUE(std::isnan(JSValue::ToNumber(JSValue(nan)).GetDouble()));
  EXPECT_TRUE(std::isinf(JSValue::ToNumber(JSValue(inf)).GetDouble()));

  { 
    auto source = uR"(
           42
 )";
    auto val = JSValue(ObjectFactory::NewString(source));
    auto num = JSValue::ToNumber(val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(42, num.GetInt());
  }

  { 
    auto source = uR"(
           42E-2
 )";
    auto val = JSValue(ObjectFactory::NewString(source));
    auto num = JSValue::ToNumber(val);

    ASSERT_TRUE(num.IsDouble());
    EXPECT_EQ(0.42, num.GetDouble());
  }


  { 
    auto source = uR"(
           42E.-2
 )";
    auto val = JSValue(ObjectFactory::NewString(source));
    auto num = JSValue::ToNumber(val);

    ASSERT_TRUE(num.IsDouble());
    EXPECT_TRUE(std::isnan(num.GetDouble()));
  }

  { 
    auto source = uR"(
           .142857E6
 )";
    auto val = JSValue(ObjectFactory::NewString(source));
    auto num = JSValue::ToNumber(val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(142857, num.GetInt());
  }

  { 
    auto source = uR"(
23.142857E3
 )";
    auto val = JSValue(ObjectFactory::NewString(source));
    auto num = JSValue::ToNumber(val);

    ASSERT_TRUE(num.GetDouble());
    EXPECT_DOUBLE_EQ(23142.857, num.GetDouble());
  }

  { 
    auto source = uR"(
0xFAb4
 )";
    auto val = JSValue(ObjectFactory::NewString(source));
    auto num = JSValue::ToNumber(val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(64180, num.GetInt());
  }
}

TEST(JSValue, ToString) {
  EXPECT_EQ(u"undefined",
            JSValue::ToString(JSValue::Undefined()).GetHeapObject()->AsString()->GetString());
  EXPECT_EQ(u"null",
            JSValue::ToString(JSValue::Null()).GetHeapObject()->AsString()->GetString());
  EXPECT_EQ(u"false",
            JSValue::ToString(JSValue::False()).GetHeapObject()->AsString()->GetString());
  EXPECT_EQ(u"true",
            JSValue::ToString(JSValue::True()).GetHeapObject()->AsString()->GetString());

  { 
    auto val = JSValue(42);
    auto str = JSValue::ToString(val);

    ASSERT_TRUE(str.IsString());
    EXPECT_EQ(u"42", str.GetHeapObject()->AsString()->GetString());
  }

  { 
    auto val = JSValue(0.142857);
    auto str = JSValue::ToString(val);

    ASSERT_TRUE(str.IsString());
    EXPECT_EQ(u"0.142857", str.GetHeapObject()->AsString()->GetString());
  }

  { 
    auto val = JSValue(-4200);
    auto str = JSValue::ToString(val);

    ASSERT_TRUE(str.IsString());
    EXPECT_EQ(u"-4200", str.GetHeapObject()->AsString()->GetString());
  }
}

