#ifndef VOIDJS_IR_LITERAL_H
#define VOIDJS_IR_LITERAL_H

#include <string_view>
#include <type_traits>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"

namespace voidjs {
namespace ast {

class NullLiteral : public Expression {
 public:
  NullLiteral()
    : Expression(AstNodeType::NULL_LITERAL)
  {}
};

class BooleanLiteral : public Expression {
 public:
  explicit BooleanLiteral(bool boolean)
    : Expression(AstNodeType::BOOLEAN_LITERAL),
      boolean_(boolean)
  {}

  bool GetBoolean() const { return boolean_; }

 private:
  bool boolean_;
};

class NumericLiteral : public Expression {
 public:
  explicit NumericLiteral(double number)
    : Expression(AstNodeType::NUMERIC_LITERAL),
      number_(number)
  {}

  template <typename T, typename = std::is_arithmetic<T>>
  T GetNumber() const { return static_cast<T>(number_); }

 private:
  double number_; 
};

class StringLiteral : public Expression {
 public:
  explicit StringLiteral(std::u16string str)
    : Expression(AstNodeType::STRING_LITERAL),
      string_(std::move(str))
  {}

  std::u16string_view GetString() const { return string_; }

 private:
  std::u16string string_;
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_LITERAL_H
