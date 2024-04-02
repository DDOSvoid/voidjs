#ifndef VOIDJS_IR_LITERAL_H
#define VOIDJS_IR_LITERAL_H

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"

namespace voidjs {
namespace ast {

class BooleanLiteral;


class Literal : public Expression {
 public:
  explicit Literal(AstNodeType type)
    : Expression(type) {}

  bool IsLiteral() const override { return true; }

  bool GetBoolean() const;
  uint32_t GetInt() const;
  double GetDouble() const;
  std::string GetString() const;
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_LITERAL_H
