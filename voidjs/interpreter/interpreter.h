#ifndef VOIDJS_INTERPRETER_INTERPRETER_H
#define VOIDJS_INTERPRETER_INTERPRETER_H

#include <variant>

#include "voidjs/lexer/token.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/interpreter/vm.h"

namespace voidjs {

class Interpreter {
 public:
  Interpreter()
    : vm_(new VM{})
  {}
  
  types::Completion Execute(ast::AstNode* ast_node);
  void InitializeBuiltinObjects();
  void EnterGlobalCode(ast::AstNode* ast_node);
  void EnterEvalCode();
  void EnterFunctionCode();
  void DeclarationBindingInstantiation(ast::AstNode* ast_node);

  types::Completion EvalProgram(ast::AstNode* ast_node);

  types::Completion EvalStatement(ast::Statement* stmt);
  types::Completion EvalBlockStatement(ast::BlockStatement* block_stmt);
  types::Completion EvalVariableStatement(ast::VariableStatement* var_stmt);
  types::Completion EvalEmptyStatement(ast::EmptyStatement* empty_stmt); 
  types::Completion EvalExpressionStatement(ast::ExpressionStatement* expr_stmt);
  types::Completion EvalDoWhileStatement(ast::DoWhileStatement* do_while_stmt);
  types::Completion EvalWhileStatement(ast::WhileStatement* while_stmt);
  types::Completion EvalForStatement(ast::ForStatement* for_stmt);
  types::Completion EvalForInStatement(ast::ForInStatement* for_in_stmt); 
  types::Completion EvalIfStatement(ast::IfStatement* if_stmt);
  types::Completion EvalFunctionDeclaration(ast::AstNode* ast_node);

  std::variant<JSValue, types::Reference> EvalExpression(ast::Expression* expr);
  std::variant<JSValue, types::Reference> EvalSequenceExpression(ast::SequenceExpression* seq_aexpr);
  std::variant<JSValue, types::Reference> EvalAssignmentExpression(ast::AssignmentExpression* assign_expr);
  std::variant<JSValue, types::Reference> EvalConditionalExpression(ast::ConditionalExpression* cond_expre);
  std::variant<JSValue, types::Reference> EvalBinaryExpression(ast::BinaryExpression* binary_expr);
  std::variant<JSValue, types::Reference> EvalUnaryExpression(ast::UnaryExpression* unary_expr);
  std::variant<JSValue, types::Reference> EvalPostfixExpression(ast::PostfixExpression* post_expr);
  std::variant<JSValue, types::Reference> EvalMemberExpression(ast::MemberExpression* ast_node);
  JSValue EvalObjectLiteral(ast::ObjectLiteral* object); 
  JSValue EvalNullLiteral(ast::NullLiteral* nul);
  JSValue EvalBooleanLiteral(ast::BooleanLiteral* boolean);
  JSValue EvalNumericLiteral(ast::NumericLiteral* num);
  JSValue EvalStringLiteral(ast::StringLiteral* str);
  types::Reference EvalIdentifier(ast::Identifier* ident);
  
  types::Completion EvalStatementList(const ast::Statements& stmts);
  void EvalVariableDeclarationList(const ast::VariableDeclarations& decls);
  JSValue EvalVariableDeclaration(ast::VariableDeclaration* decl); 
  
  JSValue ApplyCompoundAssignment(TokenType op, JSValue lval, JSValue rval);
  JSValue ApplyLogicalOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyBitwiseOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyEqualityOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyRelationalOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyShiftOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyAdditiveOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyMultiplicativeOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSValue ApplyUnaryOperator(TokenType op, ast::Expression* expr);
  JSValue ApplyPostfixOperator(TokenType op, ast::Expression* expr);

  types::Reference IdentifierResolution(types::String* ident);
  bool AbstractEqualityComparison(JSValue x, JSValue y);
  bool StrictEqualityComparison(JSValue x, JSValue y);
  JSValue AbstractRelationalComparison(JSValue x, JSValue y, bool left_first); 
  
  JSValue GetValue(const std::variant<JSValue, types::Reference>& V);
  void PutValue(const std::variant<JSValue, types::Reference>& V, JSValue W);
  void Put(JSValue base, JSValue P, JSValue W, bool Throw);

  VM* GetVm() const { return vm_; }

 private:
  VM* vm_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_INTERPRETER_H
