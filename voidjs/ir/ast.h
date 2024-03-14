#ifndef VOIDJS_IR_AST_H
#define VOIDJS_IR_AST_H

#include <vector>

namespace voidjs {
namespace ast {

enum class AstNodeType {
  PROGRAM,
  STATEMENT,
  BLOCK_STATEMENT,
  EMPTY_STATEMENT,
};

class AstNode;
class Program;
class Statement;
class Expression;

using Statements = std::vector<Statement*>;

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
  bool IsEmptyStatement() { return type_ == AstNodeType::EMPTY_STATEMENT; }
  
 private:
  AstNodeType type_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_AST_H
