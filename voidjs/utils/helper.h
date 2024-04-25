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

// This code comes from https://github.com/zhuzilin/es/blob/main/es/utils/helper.h
inline std::u16string U8StrToU16Str(std::string u8str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.from_bytes(u8str);
}

// This code comes from https://github.com/zhuzilin/es/blob/main/es.cc
inline std::string U16StrToU8Str(std::u16string u16str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(u16str);
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

static constexpr std::uint64_t DOUBLE_SIGNIFICAND_BITS = 52;
static constexpr std::uint64_t DOUBLE_SIGNIFICAND_MASK = 0x000'FFFFFFFFFFFFF;

static constexpr std::uint64_t DOUBLE_HIDDEN_BIT       = 1ull << DOUBLE_SIGNIFICAND_BITS;

static constexpr std::uint64_t DOUBLE_EXPONENT_BITS    = 11;
static constexpr std::uint64_t DOUBLE_EXPONENT_MASK    = 0x7FF'0000000000000;
static constexpr std::uint64_t DOUBLE_EXPONENT_BIAS    = (1ull << (DOUBLE_EXPONENT_BITS - 1)) - 1;

static constexpr std::uint64_t DOUBLE_SIGN_BITS        = 1; 
static constexpr std::uint64_t DOUBLE_SIGN_MASK        = 0x800'0000000000000;

// This code comes from 
template <std::size_t BITS>
inline std::int32_t DoubleToInt(double d) {
  static_assert(BITS == 8 || BITS == 16 || BITS == 32 || BITS == 64,
                "invalid bits for DoubleToInt convert");
  int32_t ret = 0;
  auto u64 = BitCast<uint64_t>(d);
  int exp = static_cast<int>((u64 & DOUBLE_EXPONENT_MASK) >> DOUBLE_SIGNIFICAND_BITS) - DOUBLE_EXPONENT_BIAS;
  if (exp < static_cast<int>(BITS - 1)) {
    // smaller than INT<BITS>_MAX, fast conversion
    ret = static_cast<int32_t>(d);
  } else if (exp < static_cast<int>(BITS + DOUBLE_SIGNIFICAND_BITS)) {
    // Still has significand bits after mod 2^<BITS>
    // Get low <BITS> bits by shift left <64 - BITS> and shift right <64 - BITS>
    uint64_t value = (((u64 & DOUBLE_SIGNIFICAND_MASK) | DOUBLE_HIDDEN_BIT)
                      << (static_cast<uint32_t>(exp) - DOUBLE_SIGNIFICAND_BITS + sizeof(std::uint64_t) - BITS)) >>
      (sizeof(std::uint64_t) - BITS);
    ret = static_cast<int32_t>(value);
    if ((u64 & DOUBLE_SIGN_MASK) == DOUBLE_SIGN_MASK && ret != INT32_MIN) {
      ret = -ret;
    }
  } else {
    // No significand bits after mod 2^<bits>, contains NaN and INF
    ret = 0;
  }
  return ret;
}

// CanDoubleConvertToInt32
// If number is an integer in the range of std::int32_t, return true;
// otherwise, return fales.
inline bool CanDoubleConvertToInt32(double number) {
  if (std::fabs(number) <= static_cast<double>(std::numeric_limits<std::int32_t>::max())) {
    auto num = static_cast<std::int32_t>(number);
    return static_cast<double>(num) == number;
  } else {
    return false;
  }
}

inline double TruncateDouble(double d) {
    if (std::isnan(d)) {
        return 0;
    }
    if (std::isinf(d)) {
        return d;
    }
    // -0 to +0
    if (d == 0.0) {
        return 0;
    }
    double ret = d >= 0 ? std::floor(d) : std::ceil(d);
    // -0 to +0
    if (ret == 0.0) {
        ret = 0;
    }
    return ret;
}

}  // namespace utils
}  // namespace voidjs

#endif  // VOIDJS_UTILS_HELPER_H
