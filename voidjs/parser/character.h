#ifndef VOIDJS_CHARACTER_H
#define VOIDJS_CHARACTER_H

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


// End of Line
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
  return true;
}

constexpr bool IsUnicodeCombiningMark(char16_t ch) {
  return true;
}

constexpr bool IsUnicodeDigit(char16_t ch) {
  return true;
}

constexpr bool IsUnicodeConnectorPunctuation(char16_t ch) {
  return true;
}

constexpr bool IsIdentifierStart(char16_t ch) {
  return IsUnicodeLetter(ch) || ch == u'$' || ch == u'_';
}

constexpr bool IsIdentifierPart(char16_t ch) {
  return
    IsIdentifierPart(ch) || IsUnicodeCombiningMark(ch)        ||
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
    ch == u'f'  || ch == u'n'  || ch == u'r' ||
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

// Regular Expression Precidates
// Defined in ECMAScript 5.1 Chapter 7.8.5


}  // namespace character 
}  // namespace voidjs

#endif  // VOIDJS_CHARACTER_H

