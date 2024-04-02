#ifndef VOIDJS_IR_LITERAL_H
#define VOIDJS_IR_LITERAL_H

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"

namespace voidjs {
namespace ast {

class Literal : public Expression {
 public:
  explicit Literal(AstNodeType type)
    : Expression(type) {}

  bool IsLiteral() const override { return true; }
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_LITERAL_H
