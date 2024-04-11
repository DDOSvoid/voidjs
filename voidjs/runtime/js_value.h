#ifndef VOIDJS_RUNTIME_JS_VALUE_H
#define VOIDJS_RUNTIME_JS_VALUE_H

#include <cstdint>

namespace voidjs {
namespace jsvalue {

// [0x0000] [48 bit direct pointer]
inline constexpr std::uint64_t TAG_OBJECT_MASK = 0xffff'000000000000;
inline constexpr std::uint64_t TAG_OBJECT      = 0x0000'000000000000;

//    False:       [56 bits 0] | 0000 0110
//    True:        [56 bits 0] | 0000 0111
//    Undefined:   [56 bits 0] | 0000 0010
//    Null:        [56 bits 0] | 0000 0011
//    Hole:        [56 bits 0] | 0000 0101
inline constexpr std::uint64_t VALUE_FALSE     = 0x0000'0000'0000'0006;
inline constexpr std::uint64_t VALUE_TRUE      = 0x0000'0000'0000'0007;
inline constexpr std::uint64_t VALUE_UNDEFINED = 0x0000'0000'0000'0002;
inline constexpr std::uint64_t VALUE_NULL      = 0x0000'0000'0000'0003;
inline constexpr std::uint64_t VALUE_HOLE      = 0x0000'0000'0000'0005;

// qNaN
inline constexpr std::uint64_t DOUBLE_OFFSET_BIT = 48;
inline constexpr std::uint64_t DOUBLE_OFFSET = 1ull << DOUBLE_OFFSET_BIT;
inline constexpr std::uint64_t NAN_MASK = 0xFFF8'0000'0000'0000 + DOUBLE_OFFSET_BIT;

// [0xFFFF] [0x0000] [32 bit signed integer]
inline constexpr std::uint64_t TAG_INT_MASK = 0xFFFF'FFFF'00000000;
inline constexpr std::uint64_t TAG_INT      = 0xFFFF'0000'00000000;

}  // namespace jsvalue

namespace types {

class Undefined;
class Null;
class Boolean;
class String;
class Number;

}  // namespace types

class JsValue {
 public:
  constexpr JsValue()
    : value_(jsvalue::VALUE_HOLE)
  {}

  constexpr JsValue(std::uint64_t value)
    : value_(value)
  {}

  bool operator==(const JsValue& other) const { return value_ == other.value_; }
  bool operator!=(const JsValue& other) const { return value_ != other.value_; }

  bool IsFalse() const;
  bool IsTrue() const;
  bool IsUndefined() const;
  bool IsNull() const;
  bool IsHole() const;

  bool IsObject() const;
  bool IsInt() const;
  bool IsDouble() const;

 private:
  std::uint64_t value_ {};
};

}  // namespace voidjs

#endif  // VOIDJS_RUNTIME_JS_VALUE_H
