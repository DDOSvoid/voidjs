#include "voidjs/types/js_value.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <iostream>
#include <locale>

#include "voidjs/lexer/character.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_string.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/builtins/js_number.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/string_table.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/helper.h"
#include "voidjs/utils/macros.h"

namespace voidjs {

bool JSValue::IsString() const {
  return IsHeapObject() && GetHeapObject()->IsString();
}

std::u16string_view JSValue::GetString() const {
  return GetHeapObject()->AsString()->GetString();
}

// ToPrimitive
// Defined in ECMAScript 5.1 Chapter 9.1
JSHandle<JSValue> JSValue::ToPrimitive(VM* vm, JSHandle<JSValue> val, PreferredType type) {
  if (val->IsPrimitive()) {
    return val;
  } else if (val->IsObject()) {
    return types::Object::DefaultValue(vm, val.As<types::Object>(), type);
  }
  
  // this branch is unreachable
  return JSHandle<JSValue>{};
}

// ToBoolean
// Defined in ECMAScript 5.1 Chapter 9.2
bool JSValue::ToBoolean(VM* vm, JSHandle<JSValue> val) {
  if (val->IsUndefined() || val->IsNull()) {
    return false;
  } else if (val->IsBoolean()) {
    return val->GetBoolean();
  } else if (val->IsNumber()) {
    if (val->IsInt()) {
      return val->GetInt() != 0;
    } else {
      // val.IsDouble must be true
      auto d = val->GetDouble();
      return !std::isnan(d) && d != 0;
    }
  } else if (val->IsString()) {
    return val->GetString().size() != 0;
  } else if (val->IsObject()) {
    return true;
  }
  
  // this branch is unreachable
  return false;
}

// ToNumber
// Defined in ECMAScript 5.1 Chapter 9.3
types::Number JSValue::ToNumber(VM* vm, JSHandle<JSValue> val) {
  if (val->IsUndefined()) {
    return types::Number::NaN();
  } else if (val->IsNull()) {
    return types::Number{0};
  } else if (val->IsBoolean()) {
    return types::Number{val->GetBoolean() ? 1 : 0};
  } else if (val->IsNumber()) {
    return val->IsInt() ? types::Number{val->GetInt()} : types::Number{val->GetNumber()};
  } else if (val->IsString()) {
    auto num = StringToNumber(vm, val.As<types::String>());
    if (utils::CanDoubleConvertToInt32(num)) {
      return types::Number{static_cast<std::int32_t>(num)}; 
    } else {
      return types::Number{num};
    }
  } else if (val->IsObject()) {
    auto prim_val = ToPrimitive(vm, val, PreferredType::NUMBER);
    return ToNumber(vm, prim_val);
  }

  // this branch is unreachable
  return types::Number{};
}

// ToInteger
// Defined in ECMAScript 5.1 Chapter 9.4
types::Number JSValue::ToInteger(VM* vm, JSHandle<JSValue> val) {
  // 1. Let number be the result of calling ToNumber on the input argument.
  // 2. If number is NaN, return +0.
  // 3. If number is +0, −0, +∞, or −∞, return number.
  // 4. Return the result of computing sign(number) * floor(abs(number)).
  
  // auto num = ToNumber(val);
  // if (num.IsDouble() && std::isnan(num.GetDouble())) {
  //   return JSValue(0);
  // } else if (num.IsInt() && num.GetInt() == 0 ||
  //            num.IsDouble() && (std::isinf(num.GetDouble()) || num.GetDouble() == 0)) {
  //   return num;
  // } else {
  //   auto ret = num.GetDouble() > 0 ?
  //     std::floor(std::fabs(num.GetDouble())) : -std::floor(std::fabs(num.GetDouble()));
  //   return JSValue(ret);
  // }

  return types::Number{utils::TruncateDouble(ToNumber(vm, val).GetNumber())};
}

// ToInt32
// Defined in ECMAScript 5.1 Chapter 9.5
std::int32_t JSValue::ToInt32(VM* vm, JSHandle<JSValue> val) {
  // 1. Let number be the result of calling ToNumber on the input argument.
  // 2. If number is NaN, +0, −0, +∞, or −∞, return +0.
  // 3. Let posInt be sign(number) * floor(abs(number)).
  // 4. Let int32bit be posInt modulo 2^32;
  //    that is, a finite integer value k of Number type with positive sign and
  //    less than 2^32 in magnitude such that the mathematical difference of posInt and
  //    k is mathematically an integer multiple of 2^32.
  // 5. If int32bit is greater than or equal to 2^31,
  //    return int32bit − 2^32, otherwise return int32bit.
  
  // auto num = ToNumber(val);
  
  // if (num.IsDouble() && (std::isnan(num.GetDouble()) || std::isinf(num.GetDouble()) || num.GetDouble() == 0) ||
  //     num.IsInt() && num.GetInt() == 0) {
  //   return 0;
  // }

  // double number = num.IsInt() ? num.GetInt() : num.GetDouble();
  
  // auto pos_int = number > 0?
  //   std::floor(std::fabs(number)) : -std::floor(std::fabs(number));
  
  // auto int32_bit = std::fmod(pos_int, std::pow(2, 32));
  // if (int32_bit < 0) {
  //   int32_bit += std::pow(2, 32);
  // }

  // if (int32_bit > pow(2, 31)) {
  //   return int32_bit - std::pow(2, 32);
  // } else {
  //   return int32_bit;
  // }

  return utils::DoubleToInt<32>(ToNumber(vm, val).GetNumber());
}

// ToUint32
// Defined in ECMAScript 5.1 Chapter 9.6
std::uint32_t JSValue::ToUint32(VM* vm, JSHandle<JSValue> val) {
  // 1. Let number be the result of calling ToNumber on the input argument.
  // 2. If number is NaN, +0, −0, +∞, or −∞, return +0.
  // 3. Let posInt be sign(number) * floor(abs(number)).
  // 4. Let int32bit be posInt modulo 2^32;
  //    that is, a finite integer value k of Number type with positive sign and
  //    less than 2^32 in magnitude such that the mathematical difference of posInt and
  //    k is mathematically an integer multiple of 2^32.
  // 5. Return int32bit.
  
  // auto num = ToNumber(val);

  // if (num.IsDouble() && (std::isnan(num.GetDouble()) || std::isinf(num.GetDouble()) || num.GetDouble() == 0) ||
  //     num.IsInt() && num.GetInt() == 0) {
  //   return 0;
  // }

  // double number = num.IsInt() ? num.GetInt() : num.GetDouble();

  // auto pos_int = number > 0 ? 
  //   std::floor(std::fabs(number)) : -std::floor(std::fabs(number));

  // auto int32_bit = std::fmod(pos_int, std::pow(2, 32));
  // if (int32_bit < 0) {
  //   int32_bit += std::pow(2, 32);
  // }

  // return int32_bit;

  return utils::DoubleToInt<32>(ToNumber(vm, val).GetNumber());
}

// ToUint16
// Defined in ECMAScript 5.1 Chapter 9.7
std::uint16_t JSValue::ToUint16(VM* vm, JSHandle<JSValue> val) {
  // 1. Let number be the result of calling ToNumber on the input argument.
  // 2. If number is NaN, +0, −0, +∞, or −∞, return +0.
  // 3. Let posInt be sign(number) * floor(abs(number)).
  // 4. Let int16bit be posInt modulo 2^16;
  //    that is, a finite integer value k of Number type with positive sign and
  //    less than 2^16 in magnitude such that the mathematical difference of posInt and
  //    k is mathematically an integer multiple of 2^16.
  // 5. Return int16bit.
  
  // auto num = ToNumber(val);

  // if (num.IsDouble() && (std::isnan(num.GetDouble()) || std::isinf(num.GetDouble()) || num.GetDouble() == 0) ||
  //     num.IsInt() && num.GetInt() == 0) {
  //   return 0;
  // }
  
  // double number = num.IsInt() ? num.GetInt() : num.GetDouble();

  // auto pos_int = number > 0 ?
  //   std::floor(std::fabs(number)) : -std::floor(std::fabs(number));

  // auto int16_bit = std::fmod(pos_int, std::pow(2, 16));
  // if (int16_bit < 0) {
  //   int16_bit += std::pow(2, 16);
  // }

  // return int16_bit;

  return utils::DoubleToInt<16>(ToNumber(vm, val).GetNumber());
}

// ToString
// Defined in ECMAScript 5.1 Chapter 9.8
JSHandle<types::String> JSValue::ToString(VM* vm, JSHandle<JSValue> val) {
  auto factory = vm->GetObjectFactory();
  
  if (val->IsUndefined()) {
    return vm->GetGlobalConstants()->HandledUndefinedString();
  } else if (val->IsNull()) {
    return vm->GetGlobalConstants()->HandledNullString();
  } else if (val->IsBoolean()) {
    if (val->IsTrue()) {
      return vm->GetGlobalConstants()->HandledTrueString();
    } else {
      return vm->GetGlobalConstants()->HandledFalseString();
    }
  } else if (val->IsNumber()) {
    return NumberToString(vm, val->IsInt() ? val->GetInt() : val->GetDouble());
  } else if (val->IsString()) {
    return val.As<types::String>();
  } else if (val->IsObject()) {
    auto prim_val = ToPrimitive(vm, val, PreferredType::STRING);
    return ToString(vm, prim_val);
  }

  // this branch is unreachable
  return JSHandle<types::String>{};
}

// ToObject
// Defined in ECMAScript 5.1 Chapter 9.9
JSHandle<types::Object> JSValue::ToObject(VM* vm, JSHandle<JSValue> val) {
  if (val->IsUndefined() || val->IsNull()) {
    THROW_TYPE_ERROR_AND_RETURN_HANDLE(vm, u"ToObject fails when object is Undefined or Null", types::Object);
  }
  
  if (val->IsBoolean()) {
    return types::Object::Construct(vm, vm->GetBooleanConstructor(),
                                    vm->GetGlobalConstants()->HandledUndefined(), {val}).As<types::Object>();
  }
  
  if (val->IsNumber()) {
    return types::Object::Construct(vm, vm->GetNumberConstructor(),
                                    vm->GetGlobalConstants()->HandledUndefined(), {val}).As<types::Object>();
  }
  
  if (val->IsString()) {
    return types::Object::Construct(vm, vm->GetStringConstructor(),
                                    vm->GetGlobalConstants()->HandledUndefined(), {val}).As<types::Object>();
  }
  
  if (val->IsObject()) {
    return val.As<types::Object>();
  }

  THROW_TYPE_ERROR_AND_RETURN_HANDLE(vm, u"Tobject fails when object is empty.", types::Object);
}

// StringToNumber
// Defined in ECMAScript 5.1 Chapter 9.3.1
double JSValue::StringToNumber(VM* vm, JSHandle<types::String> str) {
  auto source = str->GetString();
  std::size_t start = 0, end = source.size();

  // skip whitespace and line terminator 
  while (start < end &&
         (character::IsWhitespace(source[start]) || character::IsLineTerminator(source[start]))) {
    ++start;
  }

  while (start < end &&
         (character::IsWhitespace(source[end - 1]) || character::IsLineTerminator(source[end - 1]))) {
    --end;
  }

  if (start == end) {
    return 0;
  }

  auto ToDigit = [](char16_t ch) -> std::int32_t {
    if (u'0' <= ch && ch <= u'9') {
      return ch - u'0';
    }
    if (u'a' <= ch && ch <= u'f') {
      return ch - u'a' + 10;
    }
    if (u'A' <= ch && ch <= u'F') {
      return ch - u'A' + 10;
    }
    return 0; 
  };

  auto nan = std::numeric_limits<double>::quiet_NaN();
  auto inf = std::numeric_limits<double>::infinity();
  auto ret = 0.0;
  auto num = 0.0;
  auto base = 10.0;
  auto sign = false;
  if (source[start] == u'-') {
    sign = true;
    ++start;
  } else if (source[start] == u'+') {
    ++start;
  } else if (end - start > 2 && source[start] == u'0' &&
             (source[start + 1] == u'x' || source[start + 1] == u'X')) {
    start += 2;

    num = 0.0;
    base = 16;
    for (auto idx = start; idx < end; ++idx) {
      if (!character::IsHexDigit(source[idx])) {
        return nan;
      }
      num = num * base + ToDigit(source[idx]);
    }
    return num;
  }

  if (source.substr(start, end - start) == u"Infinity") {
    return sign ? -inf : inf;
  }

  while (start < end && character::IsDecimalDigit(source[start])) {
    num = num * base + ToDigit(source[start]);
    ++start;
  }
  ret += num;

  if (start == end) {
    return sign ? -ret : ret;
  }

  if (source[start] == u'.') {
    ++start;
    
    num = 0.0;
    base = 0.1;
    while (start < end && character::IsDecimalDigit(source[start])) {
      num += ToDigit(source[start]) * base;
      base *= 0.1;
      ++start;
    }
    ret += num;
  }

  if (start == end) {
    return sign ? -ret : ret;
  }

  if (source[start] == u'e' || source[start] == u'E') {
    ++start;
    if (start == end) {
      return nan;
    }

    bool exp_sign = false;
    if (source[start] == u'-') {
      exp_sign = true;
      ++start;
    } else if (source[start] == u'+') {
      ++start;
    }
    
    num = 0.0;
    base = 10;
    while (start < end && character::IsDecimalDigit(source[start])) {
      num = num * base + ToDigit(source[start]);
      ++start;
    }
    ret = ret * std::pow(10, exp_sign ? -num : num);
  }

  if (start == end) {
    return sign ? -ret : ret;
  }

  return nan;
}

// NumberToString
// Defined in ECMAScript 5.1 Chapter 9.8.1
// The following code comes from https://github.com/zhuzilin/es/blob/67fb4d579bb142669acd8384ea34c62cd052945c/es/types/conversion.h#L284
JSHandle<types::String> JSValue::NumberToString(VM* vm, double num) {
  auto factory = vm->GetObjectFactory();
  
  if (std::isnan(num)) {
    return vm->GetGlobalConstants()->HandledNaNString();
  }

  if (num == 0) {
    return vm->GetGlobalConstants()->HandledZeroString();
  }

  if (std::isinf(num)) {
    return std::signbit(num) ?
      vm->GetGlobalConstants()->HandledNegativeInfinityString() : 
      vm->GetGlobalConstants()->HandledPositiveInfinityString();
  }

  std::u16string sign = u"";
  if (num < 0) {
    num = -num;
    sign = u"-";
  }

  // the fraction digits, e.g. 1.23's frac_digit = 2, 4200's = -2
  std::int32_t frac_digit = 0;
  // the total digits, e.g. 1.23's k = 3, 4200's k = 2
  std::int32_t k = 0;
  // n - k = -frac_digit
  std::int32_t n = 0;
  double tmp;
  while (std::modf(num, &tmp) != 0) {
    frac_digit++;
    num *= 10;
  }
  while (num != 0 && std::fmod(num, 10) < 1e-6) {
    frac_digit--;
    num /= 10;
  }
  double s = num;
  while (num > 0.5) {
    k++;
    num /= 10;
    std::modf(num, &tmp);
    num = tmp;
  }
  n = k - frac_digit;

  std::u16string ret = u"";
  
  if (k <= n && n <= 21) {
    while (s > 0.5) {
      ret += u'0' + static_cast<char16_t>(std::fmod(s, 10));
      s /= 10;
      std::modf(s, &tmp);
      s = tmp;
    }
    std::reverse(ret.begin(), ret.end());
    ret += std::u16string(n - k, u'0');
    return factory->NewString(sign + ret);
  }
  
  if (0 < n && n <= 21) {
    for (int i = 0; i < k; i++) {
      ret += u'0' + static_cast<char16_t>(std::fmod(s, 10));
      if (i + 1 == k - n) {
        ret += u'.';
      }
      s /= 10;
      std::modf(s, &tmp);
      s = tmp;
    }
    std::reverse(ret.begin(), ret.end());
    return factory->NewString(sign + ret);
  }
  
  if (-6 < n && n <= 0) {
    for (int i = 0; i < k; i++) {
      ret += u'0' + static_cast<char16_t>(std::fmod(s, 10));
      s /= 10;
      std::modf(s, &tmp);
      s = tmp;
    }
    std::reverse(ret.begin(), ret.end());
    ret = u"0." + std::u16string(-n, u'0') + ret;
    return factory->NewString(sign + ret);
  }
  
  if (k == 1) {
    ret += u'0' + static_cast<char16_t>(s);
    ret += u"e";
    if (n - 1 > 0) {
      ret += u"+" + std::u16string(NumberToString(vm, n - 1)->GetString());
    } else {
      ret += u"-" + std::u16string(NumberToString(vm, 1 - n)->GetString());
    }
    return factory->NewString(sign + ret);
  }
  
  for (int i = 0; i < k; i++) {
    ret += u'0' + static_cast<char16_t>(std::fmod(s, 10));
    if (i + 1 == k - 1) {
      ret += u'.';
    }
    s /= 10;
    std::modf(s, &tmp);
    s = tmp;
  }
  
  ret += u"e";
  if (n - 1 > 0) {
    ret += u"+" + std::u16string(NumberToString(vm, n - 1)->GetString());
  } else {
    ret += u"-" + std::u16string(NumberToString(vm, 1 - n)->GetString());
  }
  return factory->NewString(sign + ret);
}

// Check Object Coercible
// Defined in ECMAScript 5.1 Chapter 9.10
void JSValue::CheckObjectCoercible(VM* vm, JSHandle<JSValue> obj) {
  if (obj->IsUndefined() || obj->IsNull()) {
    THROW_TYPE_ERROR_AND_RETURN_VOID(vm, u"object cannot be converted to Object when it's Undefined or Null");
  }
}

// Check if object is callable
// Defined in ECMAScript 5.1 Chapter 9.11
bool JSValue::IsCallable() const {
  return IsObject() && GetHeapObject()->GetCallable();
}

// Check if x and y are the same
// Defined in ECMAScript 5.1 Chapter 9.12
bool JSValue::SameValue(JSValue x, JSValue y) {
  // Return true if x and y refer to the same object. 
  if (x.GetRawData() == y.GetRawData()) {
    return true;
  }
  
  // If Type(x) and Type(y) is Undefined, return true.
  if (x.IsUndefined() && y.IsUndefined()) {
    return true;
  }
  
  // If Type(x) and Type(y) is Null, return true.
  if (x.IsNull() && y.IsNull()) {
    return true;
  }
  
  // If Type(x) is Boolean,
  // return true if x and y are both true or both false;
  // otherwise, return false.
  if (x.IsBoolean() && y.IsBoolean()) {
    return x.GetBoolean() == y.GetBoolean();
  }

  // If Type(x) is Number, then.
  if (x.IsNumber() && y.IsNumber()) {
    // If x is NaN and y is NaN, return true.
    // If x is +0 and y is -0, return false.
    // If x is -0 and y is +0, return false.
    // If x is the same Number value as y, return true.
    // Return false.

    if (x.IsInt() && y.IsInt()) {
      return x.GetInt() == y.GetInt();
    }
  }
  
  // If Type(x) is String,
  // then return true if x and y are exactly the same sequence of characters
  // (same length and same characters in corresponding positions);
  // otherwise, return false.
  if (x.IsString() && y.IsString()) {
    return
      x.GetHeapObject()->AsString()->GetString() ==
      y.GetHeapObject()->AsString()->GetString();
  }
  
  return false;
}

bool JSValue::SameValue(JSHandle<JSValue> x, JSHandle<JSValue> y) {
  return SameValue(x.GetJSValue(), y.GetJSValue());
}

}  // namespace voidjs
