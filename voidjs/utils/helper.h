#ifndef VOIDJS_UTILS_HELPER_H
#define VOIDJS_UTILS_HELPER_H

#include <string>
#include <codecvt>
#include <locale>
#include <cmath>
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


}  // namespace utils
}  // namespace voidjs

#endif  // VOIDJS_UTILS_HELPER_H
