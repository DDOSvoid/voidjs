#ifndef VOIDJS_IR_EXPRESSION_H
#define VOIDJS_IR_EXPRESSION_H

#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

// Expression
// Defined in ECMAScript 5.1 Chapter 11
// Expression in ES 5.1 can be broadly categorized as follows: 
// Primary Expression
// Left-Hand-Side Expression      --> member new call
// Postfix Expression
// Unary Expression
// Multiplicative Expression
// Additive Expression
// Shift Expression
// Relational Expression
// Equality Expression
// Bitwise Expression --> and or xor
// Conditional Expression
// Assignment Expression
class Expression : public AstNode {
 public:
  explicit Expression(AstNodeType type)
    : AstNode(type)
  {}

  bool IsExpression() const override { return true; }
};

class AssignmentExpression : public Expression {
 public:
  AssignmentExpression(Expression* left, Expression* right, TokenType op)
    : Expression(AstNodeType::ASSIGNMENT_EXPRESSION),
      left_(left), right_(right), operator_(op)
  {}
  
 private:
  Expression* left_;
  Expression* right_;
  TokenType operator_;
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_EXPRESSION_H
