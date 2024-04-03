#ifndef VOIDJS_IR_LITERAL_H
#define VOIDJS_IR_LITERAL_H

#include <string_view>
#include <type_traits>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"

namespace voidjs {
namespace ast {

class Literal : public Expression {
 public:
  explicit Literal(AstNodeType type)
    : Expression(type) {}

  bool IsLiteral() const override { return true; }

  bool GetBoolean() const;
  std::int32_t GetInt() const;
  double GetDouble() const;
  std::u16string_view GetString() const;
};

class NullLiteral : public Literal {
 public:
  NullLiteral()
    : Literal(AstNodeType::NULL_LITERAL)
  {}
};

class BooleanLiteral : public Literal {
 public:
  explicit BooleanLiteral(bool boolean)
    : Literal(AstNodeType::BOOLEAN_LITERAL),
      boolean_(boolean)
  {}

  bool GetBoolean() const { return boolean_; }

 private:
  bool boolean_;
};

class NumericLiteral : public Literal {
 public:
  explicit NumericLiteral(double number)
    : Literal(AstNodeType::NUMERIC_LITERAL), number_(number)
  {}

  template <typename T, typename = std::is_arithmetic<T>>
  T GetNumber() const { return static_cast<T>(number_); }

 private:
  double number_; 
};

class StringLiteral : public Literal {
 public:
  explicit StringLiteral(std::u16string str)
    : Literal(AstNodeType::STRING_LITERAL),
      string_(std::move(str))
  {}

  std::u16string_view GetString() const { return string_; }

 private:
  std::u16string string_;
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_LITERAL_H
