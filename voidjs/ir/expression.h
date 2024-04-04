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
// Bitwise Expression             --> and or xor
// Conditional Expression
// Assignment Expression
class Expression : public AstNode {
 public:
  explicit Expression(AstNodeType type)
    : AstNode(type)
  {}

  bool IsExpression() const override { return true; }
};

class ThisExpression : public Expression {
 public:
  ThisExpression()
    : Expression(AstNodeType::THIS_EXPRESSION)
  {}
};

class NewExpression : public Expression {
 public:
  NewExpression(Expression* expr, Expressions exprs)
    : Expression(AstNodeType::NEW_EXPRESSION),
      callee_(expr), arguments_(std::move(exprs))
  {}
  
 private:
  Expression* callee_;
  Expressions arguments_;
};

class CallExpression : public Expression {
 public:
  CallExpression(Expression* callee, Expressions arguments)
    : Expression(AstNodeType::CALL_EXPRESSION),
      callee_(callee), arguments_(std::move(arguments))
  {}

 private:
  Expression* callee_;
  Expressions arguments_;
};

class MemberExpression : public Expression {
 public:
  MemberExpression(Expression* object, Expression* property)
    : Expression(AstNodeType::MEMBER_EXPRESSION),
      object_(object), property_(property)
  {}

 private:
  Expression* object_;
  Expression* property_;
};

class Identifier : public Expression {
 public:
  explicit Identifier(std::u16string name)
    : Expression(AstNodeType::IDENTIFIER),
      name_(std::move(name))
  {}

  std::u16string_view GetName() const { return name_; }

 private:
  std::u16string name_;
};

class ArrayLiteral : public Expression {
 public:
  explicit ArrayLiteral(Expressions elements)
    : Expression(AstNodeType::ARRAY_LITERAL),
      elements_(std::move(elements))
  {}

 private:
  Expressions elements_;
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
