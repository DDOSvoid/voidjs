#ifndef VOIDJS_TYPES_JS_VALUE_H
#define VOIDJS_TYPES_JS_VALUE_H

#include <cstdint>

#include "voidjs/utils/helper.h"

namespace voidjs {

// 64 bit nan-boxing
using JSValueType = std::uint64_t;

namespace jsvalue {

// Special Values
// False:       [56 bits 0] | 0000 0110
// True:        [56 bits 0] | 0000 0111
// Undefined:   [56 bits 0] | 0000 0010
// Null:        [56 bits 0] | 0000 0011
// Hole:        [56 bits 0] | 0000 0101
// Exception:   [56 bits 0] | 0000 1000
inline constexpr JSValueType VALUE_FALSE            = 0x0000'0000'0000'0006;
inline constexpr JSValueType VALUE_TRUE             = 0x0000'0000'0000'0007;
inline constexpr JSValueType VALUE_UNDEFINED        = 0x0000'0000'0000'0002;
inline constexpr JSValueType VALUE_NULL             = 0x0000'0000'0000'0003;
inline constexpr JSValueType VALUE_HOLE             = 0x0000'0000'0000'0005;
inline constexpr JSValueType VALUE_EXCEPTION        = 0x0000'0000'0000'0008;
inline constexpr JSValueType SPECIAL_VALUE_MASK     = 0xFFFF'FFFF'FFFF'FFF0;
inline constexpr JSValueType SPECIAL_VALUE          = 0x0000'0000'0000'0000;

// [0x0000] [48 bit direct pointer]
inline constexpr JSValueType TAG_HEAP_OBJECT_MASK   = 0xFFFF'000000000006;
inline constexpr JSValueType TAG_HEAP_OBJECT        = 0x0000'000000000000;

inline constexpr JSValueType TAG_BOOLEAN            = 0x0000'0000'0000'0006;

// qNaN
inline constexpr JSValueType DOUBLE_OFFSET_BIT      = 48;
inline constexpr JSValueType DOUBLE_OFFSET          = 1ull << DOUBLE_OFFSET_BIT;
inline constexpr JSValueType NAN_MASK               = 0xFFF8'0000'0000'0000 + DOUBLE_OFFSET_BIT;

// [0xFFFF] [0x0000] [32 bit signed integer]
inline constexpr JSValueType TAG_INT_MASK           = 0xFFFF'FFFF'00000000;
inline constexpr JSValueType TAG_INT                = 0xFFFF'0000'00000000;

}  // namespace jsvalue

namespace types {

class Undefined;
class Null;
class Boolean;
class Number;
class String;

class Object;

}  // namespace types

class HeapObject;
class VM;
template <typename T>
class JSHandle;

enum class PreferredType : std::uint8_t {
  NUMBER,
  STRING,
};

class JSValue {
 public:
  JSValue()
    : value_(jsvalue::VALUE_HOLE)
  {}
  
  explicit JSValue(JSValueType value)
    : value_(value)
  {}

  // used to avoid implicit conversion from pointer to bool
  explicit JSValue(void*) = delete;
  
  explicit JSValue(HeapObject* value)
    : value_(reinterpret_cast<std::uintptr_t>(value))
  {}

  explicit JSValue(bool value)
    : value_(static_cast<JSValueType>(value) | jsvalue::TAG_BOOLEAN)
  {}
    
  explicit JSValue(std::int32_t value)
    : value_((value < 0 ?
              static_cast<JSValueType>(static_cast<std::uint32_t>(value)):
              static_cast<JSValueType>(value))
             | jsvalue::TAG_INT)
  {}

  explicit JSValue(std::uint32_t value) {
    if (static_cast<std::int32_t>(value) < 0) {
      value_ = JSValue(static_cast<double>(value)).value_;
    } else {
      value_ = JSValue(static_cast<std::int32_t>(value)).value_;
    }
  }

  explicit JSValue(double value)
    : value_(utils::BitCast<JSValueType>(value) + jsvalue::DOUBLE_OFFSET)
  {}

  bool operator==(const JSValue& other) const { return value_ == other.value_; }
  bool operator!=(const JSValue& other) const { return value_ != other.value_; }

  static JSValue False() { return JSValue{jsvalue::VALUE_FALSE}; }
  static JSValue True() { return JSValue{jsvalue::VALUE_TRUE}; }
  static JSValue Undefined() { return JSValue{jsvalue::VALUE_UNDEFINED}; }
  static JSValue Null() { return JSValue{jsvalue::VALUE_NULL}; }
  static JSValue Hole() { return JSValue{jsvalue::VALUE_HOLE}; }
  static JSValue Exception() { return JSValue{jsvalue::VALUE_EXCEPTION}; }

  // language types check
  bool IsFalse() const { return value_ == jsvalue::VALUE_FALSE; }
  bool IsTrue() const { return value_ == jsvalue::VALUE_TRUE; }
  bool IsUndefined() const { return value_ == jsvalue::VALUE_UNDEFINED; }
  bool IsNull() const { return value_ == jsvalue::VALUE_NULL; }
  bool IsBoolean() const { return value_ == jsvalue::VALUE_FALSE || value_ == jsvalue::VALUE_TRUE; }
  bool IsNumber() const { return IsInt() || IsDouble(); }
  bool IsString() const;
  bool IsPrimitive() const { return IsUndefined() || IsNull() || IsBoolean() || IsNumber() || IsString(); }
  bool IsObject() const { return !IsPrimitive() && !IsHole(); }

  // internal checks
  bool IsSpecial() const { return (value_ & jsvalue::SPECIAL_VALUE_MASK) == jsvalue::SPECIAL_VALUE && (value_ & 0xF); }
  bool IsHeapObject() const { return (value_ & jsvalue::TAG_HEAP_OBJECT_MASK) == jsvalue::TAG_HEAP_OBJECT; }
  bool IsInt() const { return (value_ & jsvalue::TAG_INT_MASK) == jsvalue::TAG_INT; }
  bool IsDouble() const { return !IsHeapObject() && !IsInt() && !IsSpecial(); }
  bool IsHole() const { return value_ == jsvalue::VALUE_HOLE; }
  bool IsException() const { return value_ == jsvalue::VALUE_EXCEPTION; }
  bool IsPropertyName() const { return IsString() || IsNumber(); }

  bool GetBoolean() const { return value_ == jsvalue::VALUE_TRUE; }
  double GetNumber() const { return IsInt() ? GetInt() : GetDouble(); } 
  std::int32_t GetInt() const { return static_cast<std::int32_t>(value_ & (~jsvalue::TAG_INT_MASK)); }
  double GetDouble() const { return utils::BitCast<double>(value_ - jsvalue::DOUBLE_OFFSET); }
  std::u16string_view GetString() const; 

  HeapObject* GetHeapObject() const { return reinterpret_cast<HeapObject*>(value_); }

  JSValueType GetRawData() const { return value_; }

  // Type Conversion
  // Defined in ECMAScript 5.1 Chapter 9
  static JSHandle<JSValue> ToPrimitive(VM* vm, JSHandle<JSValue> val, PreferredType type);
  static bool ToBoolean(VM* vm, JSHandle<JSValue> val);
  static types::Number ToNumber(VM* vm, JSHandle<JSValue> val); 
  static types::Number ToInteger(VM* vm, JSHandle<JSValue> val);
  static std::int32_t ToInt32(VM* vm, JSHandle<JSValue> val);
  static std::uint32_t ToUint32(VM* vm, JSHandle<JSValue> val);
  static std::uint16_t ToUint16(VM* vm, JSHandle<JSValue> val);
  static JSHandle<types::String> ToString(VM* vm, JSHandle<JSValue> val);
  static JSHandle<types::Object> ToObject(VM* vm, JSHandle<JSValue> val);
  static double StringToNumber(VM* vm, JSHandle<types::String>str);
  static JSHandle<types::String> NumberToString(VM* vm, double num);

  // Type Testing
  // Defined in ECMAScript 5.1 Chapter 9
  static void CheckObjectCoercible(VM* vm, JSHandle<JSValue> obj);
  bool IsCallable() const;
  static bool SameValue(JSValue x, JSValue y);
  static bool SameValue(JSHandle<JSValue> x, JSHandle<JSValue> y);

 private:
  JSValueType value_ {};
};

}  // namespace voidjs

#endif  // VOIDJS_TYPES_JS_VALUE_H
