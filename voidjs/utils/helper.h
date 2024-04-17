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

inline bool IsInteger(double number) {
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

}  // namespace utils
}  // namespace voidjs

#endif  // VOIDJS_UTILS_HELPER_H
