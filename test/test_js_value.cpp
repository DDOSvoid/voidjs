#include "gtest/gtest.h"

#include <limits>
#include <cmath>

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(JSValue, ToNumber) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  auto factory = vm->GetObjectFactory();
  
  auto nan = std::numeric_limits<double>::quiet_NaN();
  auto inf = std::numeric_limits<double>::infinity();
  
  EXPECT_EQ(42, JSValue::ToNumber(vm, JSValue(42)).GetInt());
  EXPECT_DOUBLE_EQ(0.01, JSValue::ToNumber(vm, JSValue(0.01)).GetDouble());
  EXPECT_TRUE(std::isnan(JSValue::ToNumber(vm, JSValue(nan)).GetDouble()));
  EXPECT_TRUE(std::isinf(JSValue::ToNumber(vm, JSValue(inf)).GetDouble()));

  { 
    auto source = uR"(
           42
 )";
    auto val = JSValue(factory->NewString(source));
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(42, num.GetInt());
  }

  { 
    auto source = uR"(
           42E-2
 )";
    auto val = JSValue(factory->NewString(source));
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsDouble());
    EXPECT_EQ(0.42, num.GetDouble());
  }


  { 
    auto source = uR"(
           42E.-2
 )";
    auto val = JSValue(factory->NewString(source));
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsDouble());
    EXPECT_TRUE(std::isnan(num.GetDouble()));
  }

  { 
    auto source = uR"(
           .142857E6
 )";
    auto val = JSValue(factory->NewString(source));
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(142857, num.GetInt());
  }

  { 
    auto source = uR"(
23.142857E3
 )";
    auto val = JSValue(factory->NewString(source));
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.GetDouble());
    EXPECT_DOUBLE_EQ(23142.857, num.GetDouble());
  }

  { 
    auto source = uR"(
0xFAb4
 )";
    auto val = JSValue(factory->NewString(source));
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(64180, num.GetInt());
  }
}

TEST(JSValue, ToInteger) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  auto inf = 1.0 / 0.0;

  {
    auto val = JSValue(inf);
    EXPECT_DOUBLE_EQ(inf, JSValue::ToInteger(vm, val).GetDouble());
  }

  {
    auto val = JSValue(-2.3);
    EXPECT_DOUBLE_EQ(-2, JSValue::ToInteger(vm, val).GetDouble());
  }
}

TEST(JSValue, ToInt32) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  {
    auto val = JSValue(std::pow(2, 32) + 3);
    EXPECT_EQ(3, JSValue::ToInt32(vm, val));
  }

  {
    auto val = JSValue(-2.3);
    EXPECT_EQ(-2, JSValue::ToInt32(vm, val));
  }
}

TEST(JSValue, ToUint32) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  auto ui32_max = std::numeric_limits<std::uint32_t>::max();
  
  {
    auto val = JSValue(std::pow(2, 32) + 3);
    EXPECT_EQ(3, JSValue::ToInt32(vm, val));
  }

  {
    auto val = JSValue(-2.3);
    EXPECT_EQ(ui32_max - 1, JSValue::ToUint32(vm, val));
  }
}

TEST(JSValue, ToUint16) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  auto ui16_max = std::numeric_limits<std::uint16_t>::max();
  
  {
    auto val = JSValue(std::pow(2, 16) + 3);
    EXPECT_EQ(3, JSValue::ToUint16(vm, val));
  }

  {
    auto val = JSValue(-2.3);
    EXPECT_EQ(ui16_max - 1, JSValue::ToUint16(vm, val));
  }
}

TEST(JSValue, ToString) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  EXPECT_EQ(u"undefined",
            JSValue::ToString(vm, JSValue::Undefined()).GetHeapObject()->AsString()->GetString());
  EXPECT_EQ(u"null",
            JSValue::ToString(vm, JSValue::Null()).GetHeapObject()->AsString()->GetString());
  EXPECT_EQ(u"false",
            JSValue::ToString(vm, JSValue::False()).GetHeapObject()->AsString()->GetString());
  EXPECT_EQ(u"true",
            JSValue::ToString(vm, JSValue::True()).GetHeapObject()->AsString()->GetString());

  { 
    auto val = JSValue(42);
    auto str = JSValue::ToString(vm, val);

    ASSERT_TRUE(str.IsString());
    EXPECT_EQ(u"42", str.GetHeapObject()->AsString()->GetString());
  }

  { 
    auto val = JSValue(0.142857);
    auto str = JSValue::ToString(vm, val);

    ASSERT_TRUE(str.IsString());
    EXPECT_EQ(u"0.142857", str.GetHeapObject()->AsString()->GetString());
  }

  { 
    auto val = JSValue(-4200);
    auto str = JSValue::ToString(vm, val);

    ASSERT_TRUE(str.IsString());
    EXPECT_EQ(u"-4200", str.GetHeapObject()->AsString()->GetString());
  }
}

