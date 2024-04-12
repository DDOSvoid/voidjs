#ifndef VOIDJS_RUNTIME_JS_VALUE_H
#define VOIDJS_RUNTIME_JS_VALUE_H

#include <cstdint>

namespace voidjs {

// 64 bit nan-boxing
using JSValueType = std::uint64_t;

namespace jsvalue {

//    False:       [56 bits 0] | 0000 0110
//    True:        [56 bits 0] | 0000 0111
//    Undefined:   [56 bits 0] | 0000 0010
//    Null:        [56 bits 0] | 0000 0011
//    Hole:        [56 bits 0] | 0000 0101
inline constexpr JSValueType VALUE_FALSE       = 0x0000'0000'0000'0006;
inline constexpr JSValueType VALUE_TRUE        = 0x0000'0000'0000'0007;
inline constexpr JSValueType VALUE_UNDEFINED   = 0x0000'0000'0000'0002;
inline constexpr JSValueType VALUE_NULL        = 0x0000'0000'0000'0003;
inline constexpr JSValueType VALUE_HOLE        = 0x0000'0000'0000'0005;

// [0x0000] [48 bit direct pointer]
inline constexpr JSValueType TAG_OBJECT_MASK   = 0xFFFF'000000000000;
inline constexpr JSValueType TAG_OBJECT        = 0x0000'000000000000;

inline constexpr JSValueType TAG_BOOLEAN       = 0x0000'0000'0000'0006;

// qNaN
inline constexpr JSValueType DOUBLE_OFFSET_BIT = 48;
inline constexpr JSValueType DOUBLE_OFFSET     = 1ull << DOUBLE_OFFSET_BIT;
inline constexpr JSValueType NAN_MASK          = 0xFFF8'0000'0000'0000 + DOUBLE_OFFSET_BIT;

// [0xFFFF] [0x0000] [32 bit signed integer]
inline constexpr JSValueType TAG_INT_MASK      = 0xFFFF'FFFF'00000000;
inline constexpr JSValueType TAG_INT           = 0xFFFF'0000'00000000;

}  // namespace jsvalue

namespace types {

class Undefined;
class Null;
class Boolean;
class String;
class Number;

}  // namespace types

class JSValue {
 public:
  constexpr JSValue()
    : value_(jsvalue::VALUE_HOLE)
  {}

  constexpr JSValue(JSValueType value)
    : value_(value)
  {}

  constexpr JSValue(bool value)
    : value_(static_cast<JSValueType>(value) | jsvalue::TAG_BOOLEAN)
  {}
    
  constexpr JSValue(std::int32_t value)
    : value_(static_cast<std::uint64_t>(value) | jsvalue::TAG_INT)
  {}

  constexpr JSValue(std::uint32_t value) {
    if (static_cast<std::int32_t>(value) < 0) {
      value_ = JSValue(static_cast<double>(value)).value_;
    } else {
      value_ = JSValue(static_cast<std::int32_t>(value)).value_;
    }
  }

  constexpr JSValue(double value)
    : value_(static_cast<JSValueType>(value) + jsvalue::DOUBLE_OFFSET)
  {}

  bool operator==(const JSValue& other) const { return value_ == other.value_; }
  bool operator!=(const JSValue& other) const { return value_ != other.value_; }

  static constexpr JSValue False() { return JSValue(jsvalue::VALUE_FALSE); }
  static constexpr JSValue True() { return JSValue(jsvalue::VALUE_TRUE); }
  static constexpr JSValue Undefined() { return JSValue(jsvalue::VALUE_UNDEFINED); }
  static constexpr JSValue Null() { return JSValue(jsvalue::VALUE_NULL); }
  static constexpr JSValue Hole() { return JSValue(jsvalue::VALUE_HOLE); }

  bool IsFalse() const { return value_ == jsvalue::VALUE_FALSE; }
  bool IsTrue() const { return value_ == jsvalue::VALUE_TRUE; }
  bool IsUndefined() const { return value_ == jsvalue::VALUE_UNDEFINED; }
  bool IsNull() const { return value_ == jsvalue::VALUE_NULL; }
  bool IsHole() const { return value_ == jsvalue::VALUE_HOLE; }

  bool IsObject() const { return (value_ & jsvalue::TAG_OBJECT_MASK) == jsvalue::TAG_OBJECT; }
  bool IsInt() const { return (value_ & jsvalue::TAG_INT_MASK) == jsvalue::TAG_INT; }
  bool IsDouble() const { return !IsObject() && !IsInt(); }

 protected:
  JSValueType value_ {};
};

}  // namespace voidjs

#endif  // VOIDJS_RUNTIME_JS_VALUE_H
