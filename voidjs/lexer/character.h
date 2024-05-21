#ifndef VOIDJS_LEXER_CHARACTER_H
#define VOIDJS_LEXER_CHARACTER_H

#include "voidjs/lexer/unicode.h"

namespace voidjs {
namespace character {

// Format-Control Characters
// Defined in ECMAScript 5.1 Chapter 7.1
inline constexpr char16_t ZWNJ = 0x200C;  // Zero width non-joiner
inline constexpr char16_t ZWJ  = 0x200D;  // Zero width joiner
inline constexpr char16_t BOM  = 0xFEFF;  // Byte Order Mark

// Whitespace Characters
// Defined in ECMAScript 5.1 Chapter 7.2
inline constexpr char16_t TAB     = 0x0009;  // Tab
inline constexpr char16_t VT      = 0x000B;  // Vertical Tab
inline constexpr char16_t FF      = 0x000C;  // Form Feed
inline constexpr char16_t SP      = 0x0020;  // Space
inline constexpr char16_t HASHx0a = 0x00A0;  // No-break space
// constexpr char16_t BOM  = 0xFEFF;  // Byte Order Mark
// USP, any other Unicode “space separator” 

// Line Terminators
// Defined in ECMAScript 5.1 Chapter 7.3
inline constexpr char16_t LF = 0x000A;  // Line Feed 
inline constexpr char16_t CR = 0x000D;  // Carriage Return
inline constexpr char16_t LS = 0x2028;  // Line Separater
inline constexpr char16_t PS = 0x2029;  // Paragrah Separater

// End of Source
// U+0000 means NULL in Unicode, here we use it to represent EOS
inline constexpr char16_t EOS = 0x0000;

// Unicode Character Categories "Space Separater"
// https://www.compart.com/en/unicode/category/Zs
constexpr bool IsUSP(char16_t ch) {
  return
    ch == 0x0020 || ch == 0x00A0 || ch == 0x1680 ||
    0x2001 <= ch && ch <= 0x200A || ch == 0x202F ||
    ch == 0x205F || ch == 0x3000;
}

constexpr bool IsWhitespace(char16_t ch) {
  return
    ch == TAB || ch == VT      || ch == FF  ||
    ch == SP  || ch == HASHx0a || ch == BOM || IsUSP(ch);
}

constexpr bool IsLineTerminator(char16_t ch) {
  return ch == LF || ch == CR || ch == LS || ch == PS;
}

// Identifier Predicates
// Defined in ECMAScript 5.1 Chapter 7.6
constexpr bool IsUnicodeLetter(char16_t ch) {
  return 1 << unicode::GetCategory(ch) & (unicode::Lu | unicode::Ll | unicode::Lt |
                                          unicode::Lm | unicode::Lo | unicode::Nl);
}

constexpr bool IsUnicodeCombiningMark(char16_t ch) {
  return 1 << unicode::GetCategory(ch) & (unicode::Mn | unicode::Mc);
}

constexpr bool IsUnicodeDigit(char16_t ch) {
  return unicode::GetCategory(ch) == unicode::DECIMAL_DIGIT_NUMBER;
}

constexpr bool IsUnicodeConnectorPunctuation(char16_t ch) {
  return unicode::GetCategory(ch) == unicode::CONNECTOR_PUNCTUATION;
}

constexpr bool IsIdentifierStart(char16_t ch) {
  return IsUnicodeLetter(ch) || ch == u'$' || ch == u'_' || ch == u'\\';
}

constexpr bool IsIdentifierPart(char16_t ch) {
  return
    IsIdentifierStart(ch) || IsUnicodeCombiningMark(ch)        ||
    IsUnicodeDigit(ch)   || IsUnicodeConnectorPunctuation(ch) ||
    ch == ZWNJ           || ch == ZWJ; 
}

// Numeric Literal Predicates
// Defined in ECMAScript 5.1 Charpter 7.8.3
constexpr bool IsDecimalDigit(char16_t ch) {
  return u'0' <= ch && ch <= u'9'; 
}

constexpr bool IsNonZeroDigit(char16_t ch) {
  return u'1' <= ch && ch <= u'9';
}

constexpr bool IsHexDigit(char16_t ch) {
  return IsDecimalDigit(ch) || (u'a' <= ch && ch <= u'f') || (u'A' <= ch && ch <= u'F');
}

// String Literal Predicates
// Defined in EcMAScript 5.1 Charpter 7.8.4
constexpr bool IsSingleEscapeCharacter(char16_t ch) {
  return
    ch == u'\'' || ch == u'"'  || ch == u'\\' || ch == u'b' ||
    ch == u'f'  || ch == u'n'  || ch == u'r'  ||
    ch == u't'  || ch == u'v';
}

constexpr bool IsEscapeCharacter(char16_t ch) {
  return IsSingleEscapeCharacter(ch) || IsDecimalDigit(ch) || ch == u'x' || ch == u'u';
}

constexpr bool IsNonEscapeCharacter(char16_t ch) {
  return !IsEscapeCharacter(ch) || !IsLineTerminator(ch); 
}

constexpr bool IsCharacterEscapeSequence(char16_t c) {
  return IsSingleEscapeCharacter(c) || IsNonEscapeCharacter(c);
}

// This code is copied directly from https://github.com/zhuzilin/es/blob/67fb4d579bb142669acd8384ea34c62cd052945c/es/parser/character.h#L166
constexpr char16_t ToLowerCase(char16_t c) {
  if ('A' <= c && c <= 'Z') {
    return c + ('a' - 'A');
  }
  // lowercase not found until 192
  if (c < 192) {
    return c;
  }
  // suppress compiler warnings
  {
    const std::size_t index = c - 192;
    if (index < unicode::kLowerCaseCache.size()) {
      return unicode::kUpperCaseCache[index];
    }
  }
  std::array<char16_t, 101>::const_iterator it =
    std::upper_bound(unicode::kLowerCaseKeys.begin(),
                     unicode::kLowerCaseKeys.end(), c) - 1;
  const int result = static_cast<int>(it - unicode::kLowerCaseKeys.begin());
  if (result >= 0) {
    bool by2 = false;
    const char16_t start = unicode::kLowerCaseKeys[result];
    char16_t end = unicode::kLowerCaseValues[result * 2];
    if ((start & 0x8000) != (end & 0x8000)) {
      end ^= 0x8000;
      by2 = true;
    }
    if (c <= end) {
      if (by2 && (c & 1) != (start & 1)) {
        return c;
      }
      const char16_t mapping = unicode::kLowerCaseValues[result * 2 + 1];
      return c + mapping;
    }
  }
  return c;
}

constexpr char16_t ToUpperCase(char16_t c) {
  if ('a' <= c && c <= 'z') {
    return c - ('a' - 'A');
  }
  // uppercase not found until 181
  if (c < 181) {
    return c;
  }

  // suppress compiler warnings
  {
    const std::size_t index = c - 181;
    if (index < unicode::kUpperCaseCache.size()) {
      return unicode::kUpperCaseCache[index];
    }
  }
  std::array<char16_t, 113>::const_iterator it =
    std::upper_bound(unicode::kUpperCaseKeys.begin(),
                     unicode::kUpperCaseKeys.end(), c) - 1;
  const int result = static_cast<int>(it - unicode::kUpperCaseKeys.begin());
  if (result >= 0) {
    bool by2 = false;
    const char16_t start = *it;
    char16_t end = unicode::kUpperCaseValues[result * 2];
    if ((start & 0x8000) != (end & 0x8000)) {
      end ^= 0x8000;
      by2 = true;
    }
    if (c <= end) {
      if (by2 && (c & 1) != (start & 1)) {
        return c;
      }
      const char16_t mapping = unicode::kUpperCaseValues[result * 2 + 1];
      return c + mapping;
    }
  }
  return c;
}

}  // namespace character 
}  // namespace voidjs

#endif  // VOIDJS_LEXER_CHARACTER_H

