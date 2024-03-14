#ifndef VOIDJS_IR_STATEMENT_H
#define VOIDJS_IR_STATEMENT_H

#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

// Statement
// Defined in ECMAScript 5.1 Chapter 12
// Statement ::
//   Block                       --> why not BlockStatement ???
//   VariableStatement
//   EmptyStatement
//   ExpressionStatement
//   IfStatement
//   IterationStatement
//   ContinueStatement
//   BreakStatement
//   ReturnStatement
//   WithStatement
//   LabelledStatement
//   SwitchStatement
//   ThrowStatement
//   TryStatement
//   DebuggerStatement
class Statement : public AstNode {
 public:
  explicit Statement(AstNodeType type)
    : AstNode(type)
  {}
};

class BlockStatement : public Statement {
 public:
  explicit BlockStatement(Statements statements)
    : Statement(AstNodeType::BLOCK_STATEMENT), statements_(std::move(statements))
  {}
  
  void Append(Statement* stmt) {
    statements_.push_back(stmt);
  }
  
  void Assign(Statements stmts) {
    statements_.swap(stmts);
  }

 private:
  Statements statements_;
};

class EmptyStatement : public Statement {
 public:
  explicit EmptyStatement()
    : Statement(AstNodeType::EMPTY_STATEMENT)
  {}
};

class ExpressionStatement : public Statement {
 public:

 private:
  
};

}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_STATEMENT_H
