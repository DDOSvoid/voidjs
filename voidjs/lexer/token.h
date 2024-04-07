#ifndef VOIDJS_LEXER_TOKEN_H
#define VOIDJS_LEXER_TOKEN_H

#include <string>

#include "voidjs/lexer/token_type.h"

namespace voidjs {

class Token {
 public:
  Token() = default;
  Token(TokenType type, std::u16string str = u"", double num = 0.0)
    : type_(type), string_(str), number_(num)
  {}

  ~Token() = default;

  void SetType(TokenType type) { type_ = type; }
  TokenType GetType() const { return type_;}

  void SetString(std::u16string str) { string_ = std::move(str); }
  const std::u16string& GetString() const { return string_; }

  void SetNumber(double number) { number_ = number; }
  double GetNumber() const { return number_; }

  std::int32_t GetPrecedence() const;

  bool IsKeyword() const;
  bool IsIdentifierName() const;
  bool IsBinaryOperator() const;
  bool IsAssignmentOperator() const;

  bool HasString() const;
  
 private:
  TokenType type_ {TokenType::EOS};
  std::u16string string_;
  double number_ {};
};
 
}  // namespace voidjs

#endif  // VOIDJS_LEXER_TOKEN_H
