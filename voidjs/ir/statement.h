#ifndef VOIDJS_IR_STATEMENT_H
#define VOIDJS_IR_STATEMENT_H

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include <initializer_list>

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

  void Dump(Dumper* dumper) const override;

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

  void Dump(Dumper* dumper) const override;
  
 private:
  VariableDeclarations declarations_;
};

class EmptyStatement : public Statement {
 public:
  explicit EmptyStatement()
    : Statement(AstNodeType::EMPTY_STATEMENT)
  {}

  void Dump(Dumper* dumper) const override;
};

class ExpressionStatement : public Statement {
 public:
  explicit ExpressionStatement(Expression* expression)
    : Statement(AstNodeType::EXPRESSION_STATEMENT),
      expression_(expression)
  {}

  Expression* GetExpression() const { return expression_; }

  void Dump(Dumper* dumper) const override;
  
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

  void Dump(Dumper* dumper) const override;

 private:
  Expression* condition_;
  Statement* consequent_;
  Statement* alternate_;
};

class DoWhileStatement : public Statement {
 public:
  DoWhileStatement(Expression* condition, Statement* body)
    : Statement(AstNodeType::DO_WHILE_STATEMENT),
      condition_(condition), body_(body)
  {}

  Expression* GetCondition() const { return condition_; }
  Statement* GetBody() const { return body_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* condition_;
  Statement* body_;
};

class WhileStatement : public Statement {
 public:
  WhileStatement(Expression* condition, Statement* body)
    : Statement(AstNodeType::WHILE_STATEMENT),
      condition_(condition), body_(body)
  {}

  Expression* GetCondition() const { return condition_; }
  Statement* GetBody() const { return body_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* condition_;
  Statement* body_;
};

class ForStatement : public Statement {
 public:
  ForStatement(AstNode* initializer, Expression* condition,
               Expression* update, Statement* body)
    : Statement(AstNodeType::FOR_STATEMENT),
      initializer_(initializer), condition_(condition),
      update_(update), body_(body)
  {}

  AstNode* GetInitializer() const { return initializer_; }
  Expression* GetCondition() const { return condition_; }
  Expression* GetUpdate() const { return update_; }
  Statement* GetBody() const { return body_; }

  void Dump(Dumper* dumper) const override;

 private:
  AstNode* initializer_;  // VariableStatement or Expression
  Expression* condition_;
  Expression* update_;
  Statement* body_;
};

class ForInStatement : public Statement {
 public:
  ForInStatement(AstNode* left, Expression* right, Statement* body)
    : Statement(AstNodeType::FOR_IN_STATEMENT),
      left_(left), right_(right), body_(body)
  {}

  AstNode* GetLeft() const { return left_; }
  Expression* GetRight() const { return right_; }
  Statement* GetBody() const { return body_; }

  void Dump(Dumper* dumper) const override;

 private:
  AstNode* left_;  // VariableDeclaration or LeftHandSideExpression
  Expression* right_;
  Statement* body_;
};

class ContinueStatement : public Statement {
 public:
  explicit ContinueStatement(Expression* identifier)
    : Statement(AstNodeType::CONTINUE_STATEMENT),
      identifier_(identifier)
  {}

  Expression* GetIdentifier() const { return identifier_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* identifier_;
};

class BreakStatement : public Statement {
 public:
  explicit BreakStatement(Expression* identifier)
    : Statement(AstNodeType::BREAK_STATEMENT),
      identifier_(identifier)
  {}

  Expression* GetIdentifier() const { return identifier_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* identifier_;
};

class ReturnStatement : public Statement {
 public:
  explicit ReturnStatement(Expression* expression)
    : Statement(AstNodeType::RETURN_STATEMENT),
      expression_(expression)
  {}

  Expression* GetExpression() const { return expression_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* expression_;
};

class WithStatement : public Statement {
 public:
  WithStatement(Expression* context, Statement* body)
    : Statement(AstNodeType::WITH_STATEMENT),
      context_(context), body_(body)
  {}

  Expression* GetContext() const { return context_; }
  Statement* GetBody() const { return body_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* context_;
  Statement* body_;
};

class SwitchStatement : public Statement {
 public:
  SwitchStatement(Expression* discriminant, CaseClauses case_clauses)
    : Statement(AstNodeType::SWITCH_STATEMENT),
      discriminant_(discriminant), case_clauses_(case_clauses)
  {}

  Expression* GetDiscriminant() const { return discriminant_; }
  const CaseClauses& GetCaseClauses() const { return case_clauses_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* discriminant_;
  CaseClauses case_clauses_;
};

class LabelledStatement : public Statement {
 public:
  LabelledStatement(Expression* label, Statement* body)
    : Statement(AstNodeType::LABELLED_STATEMENT),
      label_(label), body_(body)
  {}

  Expression* GetLabel() const { return label_; }
  Statement* GetBody() const { return body_; }

  void Dump(Dumper* dumper) const override;
  
 private:
  Expression* label_;
  Statement* body_;
};

class ThrowStatement : public Statement {
 public:
  explicit ThrowStatement(Expression* expression)
    : Statement(AstNodeType::THROW_STATEMENT),
      expression_(expression)
  {}

  Expression* GetExpression() const { return expression_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* expression_;
};

class TryStatement : public Statement {
 public:
  TryStatement(Statement* body, Expression* catch_name,
               Statement* catch_block, Statement* finally_block)
    : Statement(AstNodeType::TRY_STATEMENT),
      body_(body), catch_name_(catch_name),
      catch_block_(catch_block), finally_block_(finally_block)
  {}

  Statement* GetBody() const { return body_; }
  Expression* GetCatchName() const { return catch_name_; }
  Statement* GetCatchBlock() const { return catch_block_; }
  Statement* GetFinallyBlock() const { return finally_block_; }

  void Dump(Dumper* dumper) const override;
  
 private:
  Statement* body_;
  Expression* catch_name_;
  Statement* catch_block_;
  Statement* finally_block_;
};

class DebuggerStatement : public Statement {
 public:
  DebuggerStatement()
    : Statement(AstNodeType::DEBUGGER_STATEMENT)
  {}

  void Dump(Dumper* dumper) const override;
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
  Expression* GetInitializer() const { return initializer_; }

  void Dump(Dumper* dumper) const override;
  
 private:
  Expression* identifier_;
  Expression* initializer_;
};

class CaseClause : public Statement {
 public:
  CaseClause(Expression* condition, Statements statements)
    : Statement(AstNodeType::CASE_CLAUSE), 
      condition_(condition), statements_(std::move(statements))
  {}

  Expression* GetCondition() const { return condition_; }
  Statements GetStatements() const { return statements_; }

  bool IsDefault() const { return condition_ == nullptr; }

  void Dump(Dumper* dumper) const override;
      
 private:
  Expression* condition_;
  Statements statements_;
};

class FunctionDeclaration : public Statement {
 public:
  FunctionDeclaration(Expression* name, Expressions parameters,
                      Statements statements, bool is_strict,
                      VariableDeclarations var_decls,
                      FunctionDeclarations func_decls)
    : Statement(AstNodeType::FUNCTION_DECLARATION),
      name_(name), parameters_(parameters),
      statements_(std::move(statements)), is_strict_(is_strict),
      variable_declarations_(std::move(var_decls)),
      function_declarations_(std::move(func_decls))
  {}

  Expression* GetName() const { return name_; }
  const Expressions& GetParameters() const { return parameters_; }
  const Statements& GetStatements() const { return statements_; }
  const VariableDeclarations& GetVariableDeclarations() const { return variable_declarations_; }
  const FunctionDeclarations& GetFunctionDeclarations() const { return function_declarations_; }

  bool IsStrict() const { return is_strict_; }

  void Dump(Dumper* dumper) const override;

 private:
  Expression* name_;
  Expressions parameters_;
  Statements statements_;
  bool is_strict_ {};
  
  VariableDeclarations variable_declarations_;
  FunctionDeclarations function_declarations_;
};

}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_STATEMENT_H
