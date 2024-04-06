#ifndef VOIDJS_IR_AST_H
#define VOIDJS_IR_AST_H

#include <memory>
#include <vector>

namespace voidjs {
namespace ast {

enum class AstNodeType {
  PROGRAM,

  // Statement
  STATEMENT,
  BLOCK_STATEMENT,
  VARIABLE_STATEMENT,
  EMPTY_STATEMENT,
  EXPRESSION_STATEMENT,
  IF_STATEMENT,

  // Expression
  EXPRESSION,
  THIS_EXPRESSION,
  LEFT_HAND_SIDE_EXPRESSION,
  NEW_EXPRESSION,
  CALL_EXPRESSION,
  MEMBER_EXPRESSION,
  POSTFIX_EXPRESSION,
  UNARY_EXPRESSION,
  ASSIGNMENT_EXPRESSION,

  // Literal
  LITERAL,
  NULL_LITERAL,
  BOOLEAN_LITERAL,
  NUMERIC_LITERAL,
  STRING_LITERAL,

  // Others
  IDENTIFIER,
  VARIABLE_DECLARATION,
  ARRAY_LITERAL,
};

class AstNode;
class Program;
class Statement;
class BlockStatement;
class VariableStatement;
class EmptyStatement;
class ExpressionStatement;
class IfStatement;
class Expression;
class ThisExpression;
class LeftHandSideExpression;
class NewExpression;
class CallExpression;
class MemberExpression;
class PostfixExpression;
class UnaryExpression;
class AssignmentExpression;
class Literal;
class NullLiteral;
class BooleanLiteral;
class NumericLiteral;
class StringLiteral;
class Identifier;
class VariableDeclaration;
class ArrayLiteral;

using Statements = std::vector<Statement*>;
using VariableDeclarations = std::vector<VariableDeclaration*>;
using Expressions = std::vector<Expression*>;

class AstNode {
 public:
  explicit AstNode(AstNodeType type)
    : type_(type) {}
  virtual ~AstNode() = default;

  // Non-Copyable
  AstNode(const AstNode&) = delete;
  AstNode& operator=(const AstNode&) = delete;

  AstNodeType GetType() const { return type_; }

  // Is Check
  virtual bool IsProgram() const { return type_ == AstNodeType::PROGRAM; }
  virtual bool IsStatement() const { return type_ == AstNodeType::STATEMENT; }
  bool IsBlockStatement() const { return type_ == AstNodeType::BLOCK_STATEMENT; }
  bool IsVariableStatement() const { return type_ == AstNodeType::VARIABLE_STATEMENT; }
  bool IsEmptyStatement() const { return type_ == AstNodeType::EMPTY_STATEMENT; }
  bool IsExpressionStatement() const { return type_ == AstNodeType::EXPRESSION_STATEMENT; }
  bool IsIfStatement() const { return type_ == AstNodeType::IF_STATEMENT; }
  virtual bool IsExpression() const { return type_ == AstNodeType::EXPRESSION; }
  bool IsThisExpression() const { return type_ == AstNodeType::THIS_EXPRESSION; }
  bool IsLeftHandSideExpression() const { return type_ == AstNodeType::LEFT_HAND_SIDE_EXPRESSION; }
  bool IsNewExpression() const { return type_ == AstNodeType::NEW_EXPRESSION; }
  bool IsCallExpression() const { return type_ == AstNodeType::CALL_EXPRESSION; }
  bool IsMemberExpression() const { return type_ == AstNodeType::MEMBER_EXPRESSION; }
  bool IsPostfixExpression() const { return type_ == AstNodeType::POSTFIX_EXPRESSION; }
  bool IsUnaryExpression() const { return type_ == AstNodeType::UNARY_EXPRESSION; }
  bool IsAssignmentExpression() const { return type_ == AstNodeType::ASSIGNMENT_EXPRESSION; }
  virtual bool IsLiteral() const { return type_ == AstNodeType::LITERAL; }
  bool IsNullLiteral() const { return type_ == AstNodeType::NULL_LITERAL; }
  bool IsBooleanLiteral() const { return type_ == AstNodeType::BOOLEAN_LITERAL; }
  bool IsNumericLiteral() const { return type_ == AstNodeType::NUMERIC_LITERAL; }
  bool IsStringLiteral() const { return type_ == AstNodeType::STRING_LITERAL; }
  bool IsIdentifier() const { return type_ == AstNodeType::IDENTIFIER; }
  bool IsVariableDeclaraion() const { return type_ == AstNodeType::VARIABLE_DECLARATION; }
  bool IsArrayLiteral() const { return type_ == AstNodeType::ARRAY_LITERAL; }

  // As Cast
  Program* AsProgram() { return reinterpret_cast<Program*>(this); }
  Statement* AsStatement() { return reinterpret_cast<Statement*>(this); }
  BlockStatement* AsBlockStatement() { return reinterpret_cast<BlockStatement*>(this); }
  VariableStatement* AsVariableStatement() { return reinterpret_cast<VariableStatement*>(this); }
  EmptyStatement* AsEmptyStatement() { return reinterpret_cast<EmptyStatement*>(this); }
  ExpressionStatement* AsExpressionStatement() { return reinterpret_cast<ExpressionStatement*>(this); }
  IfStatement* AsIfStatement() { return reinterpret_cast<IfStatement*>(this); }
  Expression* AsExpression() { return reinterpret_cast<Expression*>(this); }
  ThisExpression* AsThisExpression() { return reinterpret_cast<ThisExpression*>(this); }
  LeftHandSideExpression* AsLeftHandSideExpression() { return reinterpret_cast<LeftHandSideExpression*>(this); }
  NewExpression* AsNewExpression() { return reinterpret_cast<NewExpression*>(this); }
  CallExpression* AsCallExpression() { return reinterpret_cast<CallExpression*>(this); }
  MemberExpression* AsMemberExpression() { return reinterpret_cast<MemberExpression*>(this); }
  PostfixExpression* AsPostfixExpression() { return reinterpret_cast<PostfixExpression*>(this); }
  UnaryExpression* AsUnaryExpression() { return reinterpret_cast<UnaryExpression*>(this); }
  AssignmentExpression* AsAssignmentExpression() { return reinterpret_cast<AssignmentExpression*>(this); }
  Literal* AsLiteral() { return reinterpret_cast<Literal*>(this); }
  NullLiteral* AsNullLiteral() { return reinterpret_cast<NullLiteral*>(this); }
  BooleanLiteral* AsBooleanLiteral() { return reinterpret_cast<BooleanLiteral*>(this); }
  NumericLiteral* AsNumericLiteral() { return reinterpret_cast<NumericLiteral*>(this); }
  StringLiteral* AsStringLiteral() { return reinterpret_cast<StringLiteral*>(this); }
  Identifier* AsIdentifier() { return reinterpret_cast<Identifier*>(this); }
  VariableDeclaration* AsVariableDeclaration() { return reinterpret_cast<VariableDeclaration*>(this); }
  ArrayLiteral* AsArrayLiteral() { return reinterpret_cast<ArrayLiteral*>(this); }

  // As Cast const-version
  const Program* AsProgram() const { return reinterpret_cast<const Program*>(this); }
  const Statement* AsStatement() const { return reinterpret_cast<const Statement*>(this); }
  const BlockStatement* AsBlockStatement() const { return reinterpret_cast<const BlockStatement*>(this); }
  const VariableStatement* AsVariableStatement() const { return reinterpret_cast<const VariableStatement*>(this); }
  const EmptyStatement* AsEmptyStatement() const { return reinterpret_cast<const EmptyStatement*>(this); }
  const ExpressionStatement* AsExpressionStatement() const { return reinterpret_cast<const ExpressionStatement*>(this); }
  const IfStatement* AsIfStatement() const { return reinterpret_cast<const IfStatement*>(this); }
  const Expression* AsExpression() const { return reinterpret_cast<const Expression*>(this); }
  const ThisExpression* AsThisExpression() const { return reinterpret_cast<const ThisExpression*>(this); }
  const LeftHandSideExpression* AsLeftHandSideExpression() const { return reinterpret_cast<const LeftHandSideExpression*>(this); }
  const NewExpression* AsNewExpression() const { return reinterpret_cast<const NewExpression*>(this); }
  const CallExpression* AsCallExpression() const { return reinterpret_cast<const CallExpression*>(this); }
  const MemberExpression* AsMemberExpression() const { return reinterpret_cast<const MemberExpression*>(this); }
  const PostfixExpression* AsPostfixExpression() const { return reinterpret_cast<const PostfixExpression*>(this); }
  const UnaryExpression* AsUnaryExpression() const { return reinterpret_cast<const UnaryExpression*>(this); }
  const AssignmentExpression* AsAssignmentExpression() const { return reinterpret_cast<const AssignmentExpression*>(this); }
  const Literal* AsLiteral() const { return reinterpret_cast<const Literal*>(this); }
  const NullLiteral* AsNullLiteral() const { return reinterpret_cast<const NullLiteral*>(this); }
  const BooleanLiteral* AsBooleanLiteral() const { return reinterpret_cast<const BooleanLiteral*>(this); }
  const NumericLiteral* AsNumericLiteral() const { return reinterpret_cast<const NumericLiteral*>(this); }
  const StringLiteral* AsStringLiteral() const { return reinterpret_cast<const StringLiteral*>(this); }
  const Identifier* AsIdentifier() const { return reinterpret_cast<const Identifier*>(this); }
  const VariableDeclaration* AsVariableDeclaration() const { return reinterpret_cast<const VariableDeclaration*>(this); }
  const ArrayLiteral* AsArrayLiteral() const { return reinterpret_cast<const ArrayLiteral*>(this); }
  
 private:
  AstNodeType type_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_AST_H
