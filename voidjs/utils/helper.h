#ifndef VOIDJS_UTILS_HELPER_H
#define VOIDJS_UTILS_HELPER_H

#include <cstdint>
#include <limits>
#include <string>
#include <codecvt>
#include <locale>
#include <cmath>
#include <type_traits>
#include <vector>

#include "voidjs/lexer/character.h"

namespace voidjs {
namespace utils {

// Copy from https://github.com/zhuzilin/es/blob/main/es/utils/helper.h
inline std::u16string U8StrToU16Str(std::string u8str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.from_bytes(u8str);
}

// Copy from https://github.com/zhuzilin/es/blob/main/es.cc
inline std::string U16StrToU8Str(std::u16string u16str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(u16str);
}

inline bool IsInt32(double number) {
  if (std::fabs(number) <= static_cast<double>(std::numeric_limits<std::int32_t>::max())) {
    auto int_num = static_cast<std::int32_t>(number);
    return static_cast<double>(int_num) == number;
  } else {
    return false;
  }
}

template <typename T, typename U>
union Data {
  T t;
  U u;
};

// replace for std::bit_cast, which is introduced in C++20
template <typename To, typename From>
inline To BitCast(const From& src) {
  static_assert(sizeof(To) == sizeof(From));
  Data<From, To> data;
  data.t = src;
  return data.u;
}

template <typename T>
inline T BitGet(const void* start, std::size_t offset) {
  return reinterpret_cast<T>(reinterpret_cast<std::uintptr_t>(start) + offset);
}


template <typename T, std::uint64_t START_BIT, std::uint64_t END_BIT>
struct BitSet {
  // T must be integer or enum
  static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
  
  // 0 <= START_BIT <= END_BIT < 64
  static_assert(0 <= START_BIT);
  static_assert(START_BIT <= END_BIT);
  static_assert(END_BIT <= 64);

  static constexpr std::uint64_t Mask() {
    return ((1ull << (END_BIT - START_BIT)) - 1) << START_BIT; 
  }
  
  static constexpr std::uint64_t Encode(T value) {
    return (static_cast<std::uint64_t>(value) << START_BIT);
  }

  static constexpr T Decode(std::uint64_t base) {
    return static_cast<T>((base & Mask()) >> START_BIT);
  }

  static constexpr void Set(std::uint64_t* ptr, T value) {
    *ptr = (*ptr & ~Mask()) | Encode(value);
  }

  static constexpr T Get(std::uint64_t base) {
    return Decode(base);
  }
};

}  // namespace utils
}  // namespace voidjs

#endif  // VOIDJS_UTILS_HELPER_H
