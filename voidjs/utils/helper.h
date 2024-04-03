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

// Assume input string is always vaild
inline double ConvertToNumber(std::u16string str) {
  double ret = 0;
  double val = 0;
  bool dot = false;
  bool sign = false;
  bool exp = false;
  bool hex = false;
  double base = 10;
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
  for (std::size_t i = 0; i < str.size(); ++i) {
    char16_t ch = str[i];
    if (ch == u'.') {
      dot = true;
      ret += val;
      val = 0;
      base = 0.1;
    } else if (ch == u'e' || ch == u'E') {
      exp = true;
      ret += val;
      val = 0;
      base = 10;
    } else if (ch == u'-' || ch == u'+') {
      if (ch == u'-') {
        sign = true;
      }
    } else if (ch == u'x' || ch == u'X') {
      hex = true;
      base = 16;
    } else {
      if (dot) {
        val += ToDigit(ch) * base;
        base *= 0.1;
      } else {
        val = val * base + ToDigit(ch);
      }
    }
  }
  if (exp) {
    ret += std::pow(10, sign ? -val : val);
  } else {
    ret += val;
  }
  return ret;
}

// Assume input string is always valid
// Copy from 
inline std::u16string ConvertToString(std::u16string source) {
  size_t pos = 1;
  std::u16string vals;
  auto ToDigit = [](char16_t ch) -> char16_t {
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
  while (pos < source.size() - 1) {
    char16_t c = source[pos];
    switch (c) {
      case u'\\': {
        pos++;
        c = source[pos];
        switch (c) {
          case u'b':
            pos++;
            vals.push_back(u'\b');
            break;
          case u't':
            pos++;
            vals.push_back(u'\t');
            break;
          case u'n':
            pos++;
            vals.push_back(u'\n');
            break;
          case u'v':
            pos++;
            vals.push_back(u'\v');
            break;
          case u'f':
            pos++;
            vals.push_back(u'\f');
            break;
          case u'r':
            pos++;
            vals.push_back(u'\r');
            break;
          case u'0': {
            pos++;
            vals.append(std::u16string(1, 0));
            break;
          }
          case u'x': {
            pos++;  // skip 'x'
            char16_t hex = 0;
            for (size_t i = 0; i < 2; i++) {
              hex *= 16;
              hex += ToDigit(source[pos]);
              pos++;
            }
            vals.append(std::u16string(1, hex));
            break;
          }
          case u'u': {
            pos++;  // skip 'u'
            char16_t hex = 0;
            for (size_t i = 0; i < 4; i++) {
              hex *= 16;
              hex += ToDigit(source[pos]);
              pos++;
            }
            vals.append(std::u16string(1, hex));
            break;
          }
          default:
            c = source[pos];
            if (character::IsLineTerminator(c)) {
              pos++;
              continue;
            }
            pos++;
            vals.append(std::u16string(1, c));
        }
        break;
      }
      default: {
        size_t start = pos;
        while (true) {
          if (pos == source.size() - 1 || source[pos] == u'\\')
            break;
          pos++;
        }
        size_t end = pos;
        if (end == source.size() - 1 && vals.size() == 0)
          return source.substr(start, end - start);
        vals.append(source.substr(start, end - start));
      }
    }
  }
  if (vals.size() == 0) {
    return {};
  } else if (vals.size() == 1) {
    return std::u16string(1, vals[0]);
  }
  return vals;
}

}  // namespace utils
}  // namespace voidjs

#endif  // VOIDJS_UTILS_HELPER_H
