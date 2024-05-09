#ifndef VOIDJS_IR_AST_H
#define VOIDJS_IR_AST_H

#include <memory>
#include <vector>

namespace voidjs {
namespace ast {

enum class AstNodeType {
  PROGRAM,

  // Statement
  STATEMENT,                   // not used
  BLOCK_STATEMENT,
  VARIABLE_STATEMENT,
  EMPTY_STATEMENT,
  EXPRESSION_STATEMENT,
  IF_STATEMENT,
  DO_WHILE_STATEMENT,
  WHILE_STATEMENT,
  FOR_STATEMENT,
  FOR_IN_STATEMENT,
  CONTINUE_STATEMENT,
  BREAK_STATEMENT,
  RETURN_STATEMENT,
  WITH_STATEMENT,
  SWITCH_STATEMENT,
  LABELLED_STATEMENT,
  THROW_STATEMENT,
  TRY_STATEMENT,
  DEBUGGER_STATEMENT,

  // Expression
  EXPRESSION,                  // not used
  LEFT_HAND_SIDE_EXPRESSION,   // not used
  NEW_EXPRESSION,
  CALL_EXPRESSION,
  MEMBER_EXPRESSION,
  POSTFIX_EXPRESSION,
  UNARY_EXPRESSION,
  BINARY_EXPRESSION,
  CONDITIONAL_EXPRESSION,
  ASSIGNMENT_EXPRESSION,
  SEQUENCE_EXPRESSION,
  FUNCTION_EXPRESSION,

  // Literal
  LITERAL,                     // not used
  NULL_LITERAL,
  BOOLEAN_LITERAL,
  NUMERIC_LITERAL,
  STRING_LITERAL,

  // Others
  THIS,
  IDENTIFIER,
  VARIABLE_DECLARATION,
  ARRAY_LITERAL,
  OBJECT_LITERAL,
  PROPERTY,
  CASE_CLAUSE,
  FUNCTION_DECLARATION,
};

class AstNode;
class Program;
class Statement;
class BlockStatement;
class VariableStatement;
class EmptyStatement;
class ExpressionStatement;
class IfStatement;
class DoWhileStatement;
class WhileStatement;
class ForStatement;
class ForInStatement;
class ContinueStatement;
class BreakStatement;
class ReturnStatement;
class WithStatement;
class SwitchStatement;
class LabelledStatement;
class ThrowStatement;
class TryStatement;
class DebuggerStatement;
class Expression;
class LeftHandSideExpression;
class NewExpression;
class CallExpression;
class MemberExpression;
class PostfixExpression;
class UnaryExpression;
class BinaryExpression;
class ConditionalExpression;
class AssignmentExpression;
class SequenceExpression;
class FunctionExpression;
class This;
class Literal;
class NullLiteral;
class BooleanLiteral;
class NumericLiteral;
class StringLiteral;
class Identifier;
class VariableDeclaration;
class ArrayLiteral;
class ObjectLiteral;
class Property;
class CaseClause;
class FunctionDeclaration;

using Statements = std::vector<Statement*>;
using Expressions = std::vector<Expression*>;
using VariableDeclarations = std::vector<VariableDeclaration*>;
using CaseClauses = std::vector<CaseClause*>;
using Properties = std::vector<Property*>;
using FunctionDeclarations = std::vector<FunctionDeclaration*>;

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
  bool IsProgram() const { return type_ == AstNodeType::PROGRAM; }
  bool IsStatement() const;
  bool IsBlockStatement() const { return type_ == AstNodeType::BLOCK_STATEMENT; }
  bool IsVariableStatement() const { return type_ == AstNodeType::VARIABLE_STATEMENT; }
  bool IsEmptyStatement() const { return type_ == AstNodeType::EMPTY_STATEMENT; }
  bool IsExpressionStatement() const { return type_ == AstNodeType::EXPRESSION_STATEMENT; }
  bool IsIfStatement() const { return type_ == AstNodeType::IF_STATEMENT; }
  bool IsDoWhileStatement() const { return type_ == AstNodeType::DO_WHILE_STATEMENT; }
  bool IsWhileStatement() const { return type_ == AstNodeType::WHILE_STATEMENT; }
  bool IsForStatement() const { return type_ == AstNodeType::FOR_STATEMENT; }
  bool IsForInStatement() const { return type_ == AstNodeType::FOR_IN_STATEMENT; }
  bool IsContinueStatement() const { return type_ == AstNodeType::CONTINUE_STATEMENT; }
  bool IsBreakStatement() const { return type_ == AstNodeType::BREAK_STATEMENT; }
  bool IsReturnStatement() const { return type_ == AstNodeType::RETURN_STATEMENT; }
  bool IsWithStatement() const { return type_ == AstNodeType::WITH_STATEMENT; }
  bool IsSwitchStatment() const { return type_ == AstNodeType::SWITCH_STATEMENT; }
  bool IsLabelledStatement() const { return type_ == AstNodeType::LABELLED_STATEMENT; }
  bool IsThrowStatement() const { return type_ == AstNodeType::THROW_STATEMENT; }
  bool IsTryStatement() const { return type_ == AstNodeType::TRY_STATEMENT; }
  bool IsDebuggerStatement() const { return type_ == AstNodeType::DEBUGGER_STATEMENT; }
  bool IsExpression() const;
  bool IsLeftHandSideExpression() const;
  bool IsNewExpression() const { return type_ == AstNodeType::NEW_EXPRESSION; }
  bool IsCallExpression() const { return type_ == AstNodeType::CALL_EXPRESSION; }
  bool IsMemberExpression() const { return type_ == AstNodeType::MEMBER_EXPRESSION; }
  bool IsPostfixExpression() const { return type_ == AstNodeType::POSTFIX_EXPRESSION; }
  bool IsUnaryExpression() const { return type_ == AstNodeType::UNARY_EXPRESSION; }
  bool IsBinaryExpression() const { return type_ == AstNodeType::BINARY_EXPRESSION; }
  bool IsConditionalExpression() const { return type_ == AstNodeType::CONDITIONAL_EXPRESSION; }
  bool IsAssignmentExpression() const { return type_ == AstNodeType::ASSIGNMENT_EXPRESSION; }
  bool IsSequenceExpression() const { return type_ == AstNodeType::SEQUENCE_EXPRESSION; }
  bool IsFunctionExpression() const { return type_ == AstNodeType::FUNCTION_EXPRESSION; }
  bool IsNullLiteral() const { return type_ == AstNodeType::NULL_LITERAL; }
  bool IsBooleanLiteral() const { return type_ == AstNodeType::BOOLEAN_LITERAL; }
  bool IsNumericLiteral() const { return type_ == AstNodeType::NUMERIC_LITERAL; }
  bool IsStringLiteral() const { return type_ == AstNodeType::STRING_LITERAL; }
  bool IsThis() const { return type_ == AstNodeType::THIS; }
  bool IsIdentifier() const { return type_ == AstNodeType::IDENTIFIER; }
  bool IsVariableDeclaraion() const { return type_ == AstNodeType::VARIABLE_DECLARATION; }
  bool IsArrayLiteral() const { return type_ == AstNodeType::ARRAY_LITERAL; }
  bool IsObjectLiteral() const { return type_ == AstNodeType::OBJECT_LITERAL; }
  bool IsProperty() const { return type_ == AstNodeType::PROPERTY; }
  bool IsCaseClause() const { return type_ == AstNodeType::CASE_CLAUSE; }
  bool IsFunctionDeclaration() const { return type_ == AstNodeType::FUNCTION_DECLARATION; }

  // As Cast
  Program* AsProgram() { return reinterpret_cast<Program*>(this); }
  Statement* AsStatement() { return reinterpret_cast<Statement*>(this); }
  BlockStatement* AsBlockStatement() { return reinterpret_cast<BlockStatement*>(this); }
  VariableStatement* AsVariableStatement() { return reinterpret_cast<VariableStatement*>(this); }
  EmptyStatement* AsEmptyStatement() { return reinterpret_cast<EmptyStatement*>(this); }
  ExpressionStatement* AsExpressionStatement() { return reinterpret_cast<ExpressionStatement*>(this); }
  IfStatement* AsIfStatement() { return reinterpret_cast<IfStatement*>(this); }
  DoWhileStatement* AsDoWhileStatement() { return reinterpret_cast<DoWhileStatement*>(this); }
  WhileStatement* AsWhileStatement() { return reinterpret_cast<WhileStatement*>(this); }
  ForStatement* AsForStatement() { return reinterpret_cast<ForStatement*>(this); }
  ForInStatement* AsForInStatement() { return reinterpret_cast<ForInStatement*>(this); }
  ContinueStatement* AsContinueStatement() { return reinterpret_cast<ContinueStatement*>(this); }
  BreakStatement* AsBreakStatement() { return reinterpret_cast<BreakStatement*>(this); }
  ReturnStatement* AsReturnStatement() { return reinterpret_cast<ReturnStatement*>(this); }
  WithStatement* AsWithStatement() { return reinterpret_cast<WithStatement*>(this); }
  SwitchStatement* AsSwitchStatement() { return reinterpret_cast<SwitchStatement*>(this); }
  LabelledStatement* AsLabelledStatement() { return reinterpret_cast<LabelledStatement*>(this); }
  ThrowStatement* AsThrowStatement() { return reinterpret_cast<ThrowStatement*>(this); }
  TryStatement* AsTryStatement() { return reinterpret_cast<TryStatement*>(this); }
  DebuggerStatement* AsDebuggerStatement() { return reinterpret_cast<DebuggerStatement*>(this); }
  Expression* AsExpression() { return reinterpret_cast<Expression*>(this); }
  LeftHandSideExpression* AsLeftHandSideExpression() { return reinterpret_cast<LeftHandSideExpression*>(this); }
  NewExpression* AsNewExpression() { return reinterpret_cast<NewExpression*>(this); }
  CallExpression* AsCallExpression() { return reinterpret_cast<CallExpression*>(this); }
  MemberExpression* AsMemberExpression() { return reinterpret_cast<MemberExpression*>(this); }
  PostfixExpression* AsPostfixExpression() { return reinterpret_cast<PostfixExpression*>(this); }
  UnaryExpression* AsUnaryExpression() { return reinterpret_cast<UnaryExpression*>(this); }
  BinaryExpression* AsBinaryExpression() { return reinterpret_cast<BinaryExpression*>(this); }
  ConditionalExpression* AsConditionalExpression() { return reinterpret_cast<ConditionalExpression*>(this); }
  AssignmentExpression* AsAssignmentExpression() { return reinterpret_cast<AssignmentExpression*>(this); }
  SequenceExpression* AsSequenceExpression() { return reinterpret_cast<SequenceExpression*>(this); }
  FunctionExpression* AsFunctionExpression() { return reinterpret_cast<FunctionExpression*>(this); }
  Literal* AsLiteral() { return reinterpret_cast<Literal*>(this); }
  NullLiteral* AsNullLiteral() { return reinterpret_cast<NullLiteral*>(this); }
  BooleanLiteral* AsBooleanLiteral() { return reinterpret_cast<BooleanLiteral*>(this); }
  NumericLiteral* AsNumericLiteral() { return reinterpret_cast<NumericLiteral*>(this); }
  StringLiteral* AsStringLiteral() { return reinterpret_cast<StringLiteral*>(this); }
  This* AsThis() { return reinterpret_cast<This*>(this); }
  Identifier* AsIdentifier() { return reinterpret_cast<Identifier*>(this); }
  VariableDeclaration* AsVariableDeclaration() { return reinterpret_cast<VariableDeclaration*>(this); }
  ArrayLiteral* AsArrayLiteral() { return reinterpret_cast<ArrayLiteral*>(this); }
  ObjectLiteral* AsObjectLiteral() { return reinterpret_cast<ObjectLiteral*>(this); }
  Property* AsProperty() { return reinterpret_cast<Property*>(this); }
  CaseClause* AsCaseClause() { return reinterpret_cast<CaseClause*>(this); }
  FunctionDeclaration* AsFunctionDeclaration() { return reinterpret_cast<FunctionDeclaration*>(this); }

  // As Cast const-version
  const Program* AsProgram() const { return reinterpret_cast<const Program*>(this); }
  const Statement* AsStatement() const { return reinterpret_cast<const Statement*>(this); }
  const BlockStatement* AsBlockStatement() const { return reinterpret_cast<const BlockStatement*>(this); }
  const VariableStatement* AsVariableStatement() const { return reinterpret_cast<const VariableStatement*>(this); }
  const EmptyStatement* AsEmptyStatement() const { return reinterpret_cast<const EmptyStatement*>(this); }
  const ExpressionStatement* AsExpressionStatement() const { return reinterpret_cast<const ExpressionStatement*>(this); }
  const IfStatement* AsIfStatement() const { return reinterpret_cast<const IfStatement*>(this); }
  const DoWhileStatement* AsDoWhileStatement() const { return reinterpret_cast<const DoWhileStatement*>(this); }
  const WhileStatement* AsWhileStatement() const { return reinterpret_cast<const WhileStatement*>(this); }
  const ForStatement* AsForStatement() const { return reinterpret_cast<const ForStatement*>(this); }
  const ForInStatement* AsForInStatement() const { return reinterpret_cast<const ForInStatement*>(this); }
  const ContinueStatement* AsContinueStatement() const { return reinterpret_cast<const ContinueStatement*>(this); }
  const BreakStatement* AsBreakStatement() const { return reinterpret_cast<const BreakStatement*>(this); }
  const ReturnStatement* AsReturnStatement() const { return reinterpret_cast<const ReturnStatement*>(this); }
  const WithStatement* AsWithStatement() const { return reinterpret_cast<const WithStatement*>(this); }
  const SwitchStatement* AsSwitchStatement() const { return reinterpret_cast<const SwitchStatement*>(this); }
  const LabelledStatement* AsLabelledStatement() const { return reinterpret_cast<const LabelledStatement*>(this); }
  const ThrowStatement* AsThrowStatement() const { return reinterpret_cast<const ThrowStatement*>(this); }
  const TryStatement* AsTryStatement() const { return reinterpret_cast<const TryStatement*>(this); }
  const DebuggerStatement* AsDebuggerStatement() const { return reinterpret_cast<const DebuggerStatement*>(this); }
  const Expression* AsExpression() const { return reinterpret_cast<const Expression*>(this); }
  const LeftHandSideExpression* AsLeftHandSideExpression() const { return reinterpret_cast<const LeftHandSideExpression*>(this); }
  const NewExpression* AsNewExpression() const { return reinterpret_cast<const NewExpression*>(this); }
  const CallExpression* AsCallExpression() const { return reinterpret_cast<const CallExpression*>(this); }
  const MemberExpression* AsMemberExpression() const { return reinterpret_cast<const MemberExpression*>(this); }
  const PostfixExpression* AsPostfixExpression() const { return reinterpret_cast<const PostfixExpression*>(this); }
  const UnaryExpression* AsUnaryExpression() const { return reinterpret_cast<const UnaryExpression*>(this); }
  const BinaryExpression* AsBinaryExpression() const { return reinterpret_cast<const BinaryExpression*>(this); }
  const ConditionalExpression* AsConditionalExpression() const { return reinterpret_cast<const ConditionalExpression*>(this); }
  const AssignmentExpression* AsAssignmentExpression() const { return reinterpret_cast<const AssignmentExpression*>(this); }
  const SequenceExpression* AsSequenceExpression() const { return reinterpret_cast<const SequenceExpression*>(this); }
  const FunctionExpression* AsFunctionExpression() const { return reinterpret_cast<const FunctionExpression*>(this); }
  const Literal* AsLiteral() const { return reinterpret_cast<const Literal*>(this); }
  const NullLiteral* AsNullLiteral() const { return reinterpret_cast<const NullLiteral*>(this); }
  const BooleanLiteral* AsBooleanLiteral() const { return reinterpret_cast<const BooleanLiteral*>(this); }
  const NumericLiteral* AsNumericLiteral() const { return reinterpret_cast<const NumericLiteral*>(this); }
  const StringLiteral* AsStringLiteral() const { return reinterpret_cast<const StringLiteral*>(this); }
  const This* AsThis() const { return reinterpret_cast<const This*>(this); }
  const Identifier* AsIdentifier() const { return reinterpret_cast<const Identifier*>(this); }
  const VariableDeclaration* AsVariableDeclaration() const { return reinterpret_cast<const VariableDeclaration*>(this); }
  const ArrayLiteral* AsArrayLiteral() const { return reinterpret_cast<const ArrayLiteral*>(this); }
  const ObjectLiteral* AsObjectLiteral() const { return reinterpret_cast<const ObjectLiteral*>(this); }
  const Property* AsProperty() const { return reinterpret_cast<const Property*>(this); }
  const CaseClause* AsCaseClause() const { return reinterpret_cast<const CaseClause*>(this); }
  const FunctionDeclaration* AsFunctionDeclaration() const { return reinterpret_cast<const FunctionDeclaration*>(this); }
  
 private:
  AstNodeType type_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_AST_H
