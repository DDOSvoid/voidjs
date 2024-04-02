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

  bool IsProgram() { return type_ == AstNodeType::PROGRAM; }
  bool IsStatement() { return type_ == AstNodeType::STATEMENT; }
  bool IsBlockStatement() { return type_ == AstNodeType::BLOCK_STATEMENT; }
  bool IsVariableStatement() { return type_ == AstNodeType::VARIABLE_STATEMENT; }
  bool IsEmptyStatement() { return type_ == AstNodeType::EMPTY_STATEMENT; }
  bool IsExpressionStatement() { return type_ == AstNodeType::EXPRESSION_STATEMENT; }
  bool IsIfStatement() { return type_ == AstNodeType::IF_STATEMENT; }
  bool IsVariableDeclaraion() { return type_ == AstNodeType::VARIABLE_DECLARATION; }
  
 private:
  AstNodeType type_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_AST_H
