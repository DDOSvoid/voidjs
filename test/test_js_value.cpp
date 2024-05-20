#include "gtest/gtest.h"

#include <limits>
#include <cmath>

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/builtins/js_string.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/builtins/js_number.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(JSValue, ToNumber) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  auto factory = vm->GetObjectFactory();
  
  auto nan = std::numeric_limits<double>::quiet_NaN();
  auto inf = std::numeric_limits<double>::infinity();

  {
    auto val = JSHandle<JSValue>{vm, JSValue{42}};
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(42, num.GetInt());
  }
  
  EXPECT_DOUBLE_EQ(0.01, JSValue::ToNumber(vm, JSHandle<JSValue>{vm, JSValue{0.01}}).GetDouble());
  EXPECT_TRUE(std::isnan(JSValue::ToNumber(vm, JSHandle<JSValue>{vm, JSValue{nan}}).GetDouble()));
  EXPECT_TRUE(std::isinf(JSValue::ToNumber(vm, JSHandle<JSValue>{vm, JSValue{inf}}).GetDouble()));

  { 
    auto source = uR"(
           42
 )";
    auto val = factory->NewString(source).As<JSValue>();
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(42, num.GetInt());
  }

  { 
    auto source = uR"(
           42E-2
 )";
    auto val = factory->NewString(source).As<JSValue>();
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsDouble());
    EXPECT_EQ(0.42, num.GetDouble());
  }


  { 
    auto source = uR"(
           42E.-2
 )";
    auto val = factory->NewString(source).As<JSValue>();
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsDouble());
    EXPECT_TRUE(std::isnan(num.GetDouble()));
  }

  { 
    auto source = uR"(
           .142857E6
 )";
    auto val = factory->NewString(source).As<JSValue>();
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.IsInt());
    EXPECT_EQ(142857, num.GetInt());
  }

  { 
    auto source = uR"(
23.142857E3
 )";
    auto val = factory->NewString(source).As<JSValue>();
    auto num = JSValue::ToNumber(vm, val);

    ASSERT_TRUE(num.GetDouble());
    EXPECT_DOUBLE_EQ(23142.857, num.GetDouble());
  }

  { 
    auto source = uR"(
0xFAb4
 )";
    auto val = factory->NewString(source).As<JSValue>();
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
    auto val = JSHandle<JSValue>{vm, JSValue(inf)};
    EXPECT_DOUBLE_EQ(inf, JSValue::ToInteger(vm, val).GetDouble());
  }

  {
    auto val = JSHandle<JSValue>{vm, JSValue(-2.3)};
    EXPECT_DOUBLE_EQ(-2, JSValue::ToInteger(vm, val).GetDouble());
  }
}

TEST(JSValue, ToInt32) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  {
    auto val = JSHandle<JSValue>{vm, JSValue{4294967296. + 3}};
    EXPECT_EQ(3, JSValue::ToInt32(vm, val));
  }

  {
    auto val = JSHandle<JSValue>{vm, JSValue{-2.3}};
    EXPECT_EQ(-2, JSValue::ToInt32(vm, val));
  }
}

TEST(JSValue, ToUint32) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  auto ui32_max = std::numeric_limits<std::uint32_t>::max();

  {
    auto val = JSHandle<JSValue>{vm, JSValue(1)};
    EXPECT_EQ(1, JSValue::ToInt32(vm, val));
  }
  
  {
    auto val = JSHandle<JSValue>{vm, JSValue(4 * std::pow(2, 32) + std::pow(2, 16) - 1352)};
    EXPECT_EQ(64184, JSValue::ToInt32(vm, val));
  }

  {
    auto val = JSHandle<JSValue>{vm, JSValue(-2.3)};
    EXPECT_EQ(ui32_max - 1, JSValue::ToUint32(vm, val));
  }
}

TEST(JSValue, ToUint16) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  auto ui16_max = std::numeric_limits<std::uint16_t>::max();
  
  {
    auto val = JSHandle<JSValue>{vm, JSValue(std::pow(2, 16) + 3)};
    EXPECT_EQ(3, JSValue::ToUint16(vm, val));
  }

  {
    auto val = JSHandle<JSValue>{vm, JSValue(-2.3)};
    EXPECT_EQ(ui16_max - 1, JSValue::ToUint16(vm, val));
  }
}

TEST(JSValue, ToString) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  
  EXPECT_EQ(u"undefined", JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue::Undefined()})->GetString());
  EXPECT_EQ(u"null", JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue::Null()})->GetString());
  EXPECT_EQ(u"false", JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue::False()})->GetString());
  EXPECT_EQ(u"true", JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue::True()})->GetString());

  { 
    auto val = JSHandle<JSValue>{vm, JSValue(42)};
    auto str = JSValue::ToString(vm, val);

    EXPECT_EQ(u"42", str->GetString());
  }

  { 
    auto val = JSHandle<JSValue>{vm, JSValue(1)};
    auto str = JSValue::ToString(vm, val);

    EXPECT_EQ(u"1", str->GetString());
  }

  { 
    auto val = JSHandle<JSValue>{vm, JSValue(0.142857)};
    auto str = JSValue::ToString(vm, val);

    EXPECT_EQ(u"0.142857", str->GetString());
  }

  { 
    auto val = JSHandle<JSValue>{vm, JSValue(-4200)};
    auto str = JSValue::ToString(vm, val);

    EXPECT_EQ(u"-4200", str->GetString());
  }
}

TEST(JSValue, ToObject) {
  Interpreter interpreter;
  auto vm = interpreter.GetVM();
  auto factory = vm->GetObjectFactory();

  {
    auto val = JSHandle<JSValue>{vm, JSValue{false}};
    auto obj = JSValue::ToObject(vm, val);

    ASSERT_TRUE(obj->IsJSBoolean());
    
    auto boolean = obj->AsJSBoolean();

    ASSERT_TRUE(boolean->GetPrimitiveValue().IsBoolean());
    EXPECT_EQ(false, boolean->GetPrimitiveValue().GetBoolean());
  }

  {
    auto val = JSHandle<JSValue>{vm, JSValue{42}};
    auto obj = JSValue::ToObject(vm, val);

    ASSERT_TRUE(obj->IsJSNumber());
    
    auto num = obj->AsJSNumber();

    ASSERT_TRUE(num->GetPrimitiveValue().IsInt());
    EXPECT_EQ(42, num->GetPrimitiveValue().GetInt());
  }
  
  {
    auto val = factory->NewString(u"Hello").As<JSValue>();
    auto obj = JSValue::ToObject(vm, val);

    ASSERT_TRUE(obj->IsJSString());
    
    auto str = obj->AsJSString();

    ASSERT_TRUE(str->GetPrimitiveValue().IsString());
    EXPECT_EQ(u"Hello", str->GetPrimitiveValue().GetHeapObject()->AsString()->GetString());
  }
}

