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
};

class NewExpression : public Expression {
 public:
  NewExpression(Expression* expr, Expressions exprs)
    : Expression(AstNodeType::NEW_EXPRESSION),
      constructor_(expr), arguments_(std::move(exprs))
  {}

  const Expression* GetConstructor() const { return constructor_; }
  const Expressions& GetArguments() const { return arguments_; }

 private:
  Expression* constructor_;
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

  const Expression* GetObject() const { return object_; }
  const Expression* GetProperty() const { return property_; }

 private:
  Expression* object_;
  Expression* property_;
};

class PostfixExpression : public Expression {
 public:
  PostfixExpression(TokenType type, Expression* expr)
    : Expression(AstNodeType::POSTFIX_EXPRESSION),
      operator_(type), expression_(expr)
  {}

  TokenType GetOperator() const { return operator_; }
  Expression* GetExpression() const { return expression_; }
  
 private:
  TokenType operator_;
  Expression* expression_;
};

class UnaryExpression : public Expression {
 public:
  UnaryExpression(TokenType unary_op, Expression* expr)
    : Expression(AstNodeType::UNARY_EXPRESSION),
      operator_(unary_op), expression_(expr)
  {}

  TokenType GetOperator() const { return operator_; }
  Expression* GetExpression() const { return expression_; }
  
 private:
  TokenType operator_;
  Expression* expression_;
};

class BinaryExpression : public Expression {
 public:
  BinaryExpression(TokenType type, Expression* left, Expression* right)
    : Expression(AstNodeType::BINARY_EXPRESSION),
      operator_(type), left_(left), right_(right)
  {}

  TokenType GetOperator() const { return operator_; }
  Expression* GetLeft() const { return left_; }
  Expression* GetRight() const { return right_; }

 private:
  TokenType operator_;
  Expression* left_;
  Expression* right_;
};

class ConditionalExpression : public Expression {
 public:
  ConditionalExpression(Expression* condition,
                        Expression* consequent,
                        Expression* alternate)
    : Expression(AstNodeType::CONDITIONAL_EXPRESSION),
      condition_(condition),
      consequent_(consequent),
      alternate_(alternate)
  {}

  Expression* GetConditional() const { return condition_; }
  Expression* GetConsequent() const { return consequent_; }
  Expression* GetAlternate() const { return alternate_; }
  
 private:
  Expression* condition_;
  Expression* consequent_;
  Expression* alternate_;
};

class AssignmentExpression : public Expression {
 public:
  AssignmentExpression(TokenType op, Expression* left, Expression* right)
    : Expression(AstNodeType::ASSIGNMENT_EXPRESSION),
      left_(left), right_(right), operator_(op)
  {}

  TokenType GetOperator() const { return operator_; }
  Expression* GetLeft() const { return left_; }
  Expression* GetRight() const { return right_; }
  
 private:
  TokenType operator_;
  Expression* left_;
  Expression* right_;
};

class SequenceExpression : public Expression {
 public:
  explicit SequenceExpression(Expressions exprs)
    : Expression(AstNodeType::SEQUENCE_EXPRESSION),
      expressions_(std::move(exprs))
  {}

  const Expressions& GetExpressions() const { return expressions_; }

 private:
  Expressions expressions_;
};

class This: public Expression {
 public:
  This()
    : Expression(AstNodeType::THIS)
  {}
};

class FunctionExpression : public Expression {
  public:
  FunctionExpression(Expression* name, Expressions parameters, Statements statements)
    : Expression(AstNodeType::FUNCTION_EXPRESSION),
      name_(name), parameters_(parameters), statements_(statements)
  {}

  Expression* GetName() const { return name_; }
  const Expressions& GetParameters() const { return parameters_; }
  const Statements& GetStatements() const { return statements_; }

 private:
  Expression* name_;
  Expressions parameters_;
  Statements statements_;
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

  const Expressions& GetElements() const { return elements_; }

 private:
  Expressions elements_;
};

class ObjectLiteral : public Expression {
 public:
  explicit ObjectLiteral(Properties properties)
    : Expression(AstNodeType::OBJECT_LITERAL),
      properties_(std::move(properties))
  {}

  const Properties& GetProperties() const { return properties_; }

 private:
  Properties properties_;
};

enum class PropertyType {
  INIT,
  GET,
  SET,
};

class Property : public Expression {
 public:
  Property(PropertyType type, Expression* key, Expression* value)
    : Expression(AstNodeType::PROPERTY),
      type_(type), key_(key), value_(value)
  {}

  PropertyType GetType() const { return type_; }
  Expression* GetKey() const { return key_; }
  Expression* GetValue() const { return value_; }
  
 private:
  PropertyType type_;
  Expression* key_;
  Expression* value_;
};

}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_EXPRESSION_H
