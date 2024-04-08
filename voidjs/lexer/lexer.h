#ifndef VOIDJS_LEXER_LEXER_H
#define VOIDJS_LEXER_LEXER_H

#include <string>
#include <optional>

#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"

namespace voidjs {

class Lexer {
 public:
  explicit Lexer(const std::u16string& src)
    : src_(src), cur_(0), nxt_(1) {
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

  void NextToken();
  Token NextRewindToken();

  bool HasLineTerminator(); 

  Token& GetToken() { return token_; }
  const Token& GetToken() const { return token_; }

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

  void ScanIdentifier();
  void ScanNumericLiteral();
  void ScanStringLiteral();

  double ConvertToNumber(std::u16string source);
  std::u16string ConvertToString(std::u16string source);
  
 private:
  std::u16string src_;
  Token token_;
  char16_t ch_ {};
  std::size_t cur_ {};
  std::size_t nxt_ {};
  bool has_line_terminator_ {};
};

}  // namespace voidjs

#endif  // VOIDJS_LEXER_LEXER_H
