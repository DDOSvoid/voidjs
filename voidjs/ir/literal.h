#ifndef VOIDJS_IR_LITERAL_H
#define VOIDJS_IR_LITERAL_H

#include <string_view>
#include <type_traits>

#include "voidjs/utils/helper.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"

namespace voidjs {
namespace ast {

class NullLiteral : public Expression {
 public:
  NullLiteral()
    : Expression(AstNodeType::NULL_LITERAL)
  {}

  void Dump(Dumper* dumper) const override;
};

class BooleanLiteral : public Expression {
 public:
  explicit BooleanLiteral(bool boolean)
    : Expression(AstNodeType::BOOLEAN_LITERAL),
      boolean_(boolean)
  {}

  bool GetBoolean() const { return boolean_; }

  void Dump(Dumper* dumper) const override;

 private:
  bool boolean_;
};

class NumericLiteral : public Expression {
 public:
  explicit NumericLiteral(double number)
    : Expression(AstNodeType::NUMERIC_LITERAL),
      number_(number)
  {}

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  T GetNumber() const { return static_cast<T>(number_); }
  std::int32_t GetInt32() const { return number_; }
  double GetDouble() const { return number_; }

  void Dump(Dumper* dumper) const override;

 private:
  double number_; 
};

class StringLiteral : public Expression {
 public:
  explicit StringLiteral(std::u16string str)
    : Expression(AstNodeType::STRING_LITERAL),
      string_(std::move(str))
  {}

  std::u16string GetString() const { return string_; }

  void Dump(Dumper* dumper) const override;

 private:
  std::u16string string_;
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_LITERAL_H
