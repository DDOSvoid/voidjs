#ifndef VOIDJS_IR_STATEMENT_H
#define VOIDJS_IR_STATEMENT_H

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"

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

  const Statements& GetStatements() const { return statements_; }

 private:
  Statements statements_;
};

class VariableStatement : public Statement {
 public:
  explicit VariableStatement(VariableDeclarations declarations)
    : Statement(AstNodeType::VARIABLE_STATEMENT),
      declarations_(std::move(declarations))
  {}

  const VariableDeclarations& GetVariableDeclarations() const { return declarations_; }

 private:
  VariableDeclarations declarations_;
};

class EmptyStatement : public Statement {
 public:
  explicit EmptyStatement()
    : Statement(AstNodeType::EMPTY_STATEMENT)
  {}
};

class ExpressionStatement : public Statement {
 public:
  explicit ExpressionStatement(Expression* expression)
    : Statement(AstNodeType::EXPRESSION_STATEMENT),
      expression_(expression)
  {}

  Expression* GetExpression() const { return expression_; }
  
 private:
  Expression* expression_;
};

class IfStatement : public Statement {
 public:
  IfStatement(Expression* condition,
              Statement* consequent,
              Statement* alternate)
    : Statement(AstNodeType::IF_STATEMENT),
      condition_(condition),
      consequent_(consequent),
      alternate_(alternate)
  {}

  Expression* GetCondition() const { return condition_; }
  Statement* GetConsequent() const { return consequent_; }
  Statement* GetAlternate() const { return alternate_; }

 private:
  Expression* condition_;
  Statement* consequent_;
  Statement* alternate_;
};

class VariableDeclaration : public Statement {
 public:
  VariableDeclaration(Expression* identifier,
                      Expression* initializer)
    : Statement(AstNodeType::VARIABLE_DECLARATION),
      identifier_(identifier),
      initializer_(initializer)
  {}

  Expression* GetIdentifier() const { return identifier_; }
  Expression* GetInitializer() const { return initializer_;} 
  
 private:
  Expression* identifier_;
  Expression* initializer_;
};

}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_STATEMENT_H
