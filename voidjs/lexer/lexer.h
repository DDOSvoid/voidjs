#ifndef VOIDJS_LEXER_LEXER_H
#define VOIDJS_LEXER_LEXER_H

#include <string>
#include <optional>

#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"

namespace voidjs {

class Lexer {
 public:
  explicit Lexer(const std::u16string& src, bool use_strict = false)
    : src_(src), cur_(0), nxt_(1), use_strict_(use_strict) {
    if (!src_.empty()) {
      ch_ = src_.at(0);
    } else {
      ch_ = character::EOS;
    }
  }
  ~Lexer() = default;

  // Non-Copyable
  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Token NextToken();

 private:
  char16_t NextChar();
  char16_t PeekChar();

  void SkipWhitespace();
  bool SkipLineTerminatorSequence();
  TokenType SkipSingleLineComment();
  TokenType SkipMultiLineComment();

  std::optional<char16_t> SkipUnicodeEscapeSequence();
  bool SkipDecimalDigits();
  bool SkipSignedInteger();
  bool SkipExponentPart();
  bool SkipDecimalIntegerLiteral();
  bool SkipHexDigits();
  bool SkipCharacterEscapeSequence();
  bool SkipHexEscapeSequence();
  bool SkipEscapeSequence();
  bool SkipSingleStringCharacter();
  bool SkipDoubleStringCharacter();

  Token ScanIdentifier();
  Token ScanNumericLiteral();
  Token ScanStringLiteral();
  
 private:
  std::u16string src_;
  char16_t ch_ {};
  std::size_t cur_ {};
  std::size_t nxt_ {};
  bool use_strict_ {};
};

}  // namespace voidjs

#endif  // VOIDJS_LEXER_LEXER_H
