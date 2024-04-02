#ifndef VOIDJS_IR_AST_H
#define VOIDJS_IR_AST_H

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
  LEFT_HAND_SIDE_EXPRESSION,
  ASSIGNMENT_EXPRESSION,

  // Literal
  LITERAL,

  // Others
  VARIABLE_DECLARATION,
};

class AstNode;
class Program;
class Statement;
class Expression;
class VariableDeclaration;

using Statements = std::vector<Statement*>;
using VariableDeclarations = std::vector<VariableDeclaration*>;

class AstNode {
 public:
  explicit AstNode(AstNodeType type)
    : type_(type) {}
  virtual ~AstNode() = default;

  // Non-Copyable
  AstNode(const AstNode&) = delete;
  AstNode& operator=(const AstNode&) = delete;

  bool IsProgram() const { return type_ == AstNodeType::PROGRAM; }
  virtual bool IsStatement() const { return type_ == AstNodeType::STATEMENT; }
  bool IsBlockStatement() const { return type_ == AstNodeType::BLOCK_STATEMENT; }
  bool IsVariableStatement() const { return type_ == AstNodeType::VARIABLE_STATEMENT; }
  bool IsEmptyStatement() const { return type_ == AstNodeType::EMPTY_STATEMENT; }
  bool IsExpressionStatement() const { return type_ == AstNodeType::EXPRESSION_STATEMENT; }
  bool IsIfStatement() const { return type_ == AstNodeType::IF_STATEMENT; }
  virtual bool IsExpression() const { return type_ == AstNodeType::EXPRESSION; }
  bool IsLeftHandSideExpression() const { return type_ == AstNodeType::LEFT_HAND_SIDE_EXPRESSION; }
  bool IsAssignmentExpression() const { return type_ == AstNodeType::ASSIGNMENT_EXPRESSION; }
  bool IsVariableDeclaraion() const { return type_ == AstNodeType::VARIABLE_DECLARATION; }
  virtual bool IsLiteral() const { return type_ == AstNodeType::LITERAL; }
  
 private:
  AstNodeType type_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_AST_H
